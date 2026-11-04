#!/usr/bin/env python3
"""Collect pre-push QA checklist bullets from AGENTS.md/CLAUDE.md and referenced Markdown."""

import json
import re
from pathlib import Path
from typing import Optional
import os
import subprocess
import urllib.parse
import urllib.request

ROOT = Path(__file__).resolve().parents[2]
STARTS = ["AGENTS.md", "CLAUDE.md"]
LINK_RE = re.compile(r"\[[^\]]+\]\(([^)]+)\)")


def is_markdown_ref(ref: str) -> bool:
    ref = ref.strip()
    if not ref or ref.startswith(("http://", "https://", "mailto:", "#")):
        return False
    path_part = ref.split("#", 1)[0].split("?", 1)[0]
    return path_part.lower().endswith(".md")


def bullets_from_text(text: str) -> list[str]:
    items = []
    current: list[str] | None = None
    for line in text.splitlines():
        if not line.strip():
            if current is not None:
                items.append(" ".join(current).strip())
                current = None
            continue
        stripped = line.strip()
        match = re.match(r"^(?:[-*+]|\d+\.)\s+(.*)$", stripped)
        if match:
            if current is not None:
                items.append(" ".join(current).strip())
            current = [match.group(1).strip()]
            continue
        if current is not None and (line.startswith((" ", "\t"))):
            current.append(stripped)
        elif current is not None:
            items.append(" ".join(current).strip())
            current = None
    if current is not None:
        items.append(" ".join(current).strip())
    return [item for item in items if item]



def git_output(project: Path, *args: str) -> Optional[str]:
    try:
        return subprocess.check_output(
            ["git", *args], cwd=project, text=True, stderr=subprocess.DEVNULL
        ).strip()
    except (OSError, subprocess.CalledProcessError):
        return None


def parse_remote(url: Optional[str]) -> Optional[tuple[str, str]]:
    if not url:
        return None
    match = re.search(r"github\.com[:/](?P<owner>[^/]+)/(?P<repo>[^/\s]+?)(?:\.git)?$", (url or "").strip())
    if not match:
        return None
    return match.group("owner"), match.group("repo")


def github_get(path: str, token: Optional[str]) -> Optional[object]:
    url = "https://api.github.com" + path
    request = urllib.request.Request(url, headers={"Accept": "application/vnd.github+json"})
    if token:
        request.add_header("Authorization", f"Bearer {token}")
    try:
        with urllib.request.urlopen(request, timeout=20) as response:
            return json.loads(response.read().decode("utf-8"))
    except Exception:
        return None


def collect_pr_comments(project: Path) -> dict[str, list[str]]:
    branch = git_output(project, "symbolic-ref", "--short", "HEAD")
    if not branch:
        return {}
    remote_url = git_output(project, "remote", "get-url", "origin")
    parsed = parse_remote(remote_url)
    if not parsed:
        return {}
    owner, repo = parsed
    token = os.environ.get("GITHUB_TOKEN") or os.environ.get("GH_TOKEN")
    pulls = github_get(
        "/repos/{}/{}/pulls?{}".format(owner, repo, urllib.parse.urlencode({"head": f"{owner}:{branch}", "state": "open"})),
        token,
    )
    if not isinstance(pulls, list) or not pulls:
        return {}
    pr_number = pulls[0].get("number")
    if not pr_number:
        return {}

    items: list[str] = []
    seen: set[str] = set()

    def add_comment(comment: dict) -> None:
        body = " ".join((comment.get("body") or "").split())
        if not body:
            return
        if len(body) > 1000:
            body = body[:997] + "..."
        user = comment.get("user") or {}
        author = user.get("login", "unknown")
        path = comment.get("path")
        where = f" ({path})" if path else ""
        label = f"{author}{where}:"
        bull = f"{label} {body}"
        if bull in seen:
            return
        seen.add(bull)
        items.append(bull)

    for comment in github_get(f"/repos/{owner}/{repo}/pulls/{pr_number}/comments", token) or []:
        if isinstance(comment, dict):
            add_comment(comment)
    for comment in github_get(f"/repos/{owner}/{repo}/issues/{pr_number}/comments", token) or []:
        if isinstance(comment, dict):
            add_comment(comment)
    for review in github_get(f"/repos/{owner}/{repo}/pulls/{pr_number}/reviews", token) or []:
        if isinstance(review, dict) and review.get("body"):
            add_comment({"body": review.get("body"), "user": review.get("user")})

    if not items:
        return {}
    return {f"PR #{pr_number} comments": items}


def main() -> None:
    project = ROOT
    visited: set[Path] = set()
    queue: list[Path] = []
    for name in STARTS:
        path = project / name
        if path.is_file():
            queue.append(path)

    sources: dict[str, list[str]] = {}
    while queue:
        path = queue.pop(0)
        try:
            resolved = path.resolve()
        except OSError:
            continue
        if resolved in visited:
            continue
        visited.add(resolved)

        if not resolved.is_file():
            continue
        try:
            text = resolved.read_text(encoding="utf-8")
        except (OSError, UnicodeDecodeError):
            continue

        source = str(resolved.relative_to(project))
        items = bullets_from_text(text)
        if items:
            sources.setdefault(source, []).extend(items)

        for match in LINK_RE.finditer(text):
            ref = match.group(1)
            if not is_markdown_ref(ref):
                continue
            ref_path = (resolved.parent / ref.split("#", 1)[0]).resolve()
            if ref_path not in visited and project in ref_path.parents:
                queue.append(ref_path)

    pr_comments = collect_pr_comments(project)
    for source, items in pr_comments.items():
        sources.setdefault(source, []).extend(items)

    output_path = project / ".claude" / "pre-push-checklist.md"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Pre-Push QA Checklist",
        "",
        "Source: AGENTS.md, CLAUDE.md, and Markdown files referenced by them.",
        "",
    ]
    total = 0
    for source, items in sorted(sources.items()):
        lines.append(f"## Source: {source}")
        lines.append("")
        for index, item in enumerate(items, 1):
            total += 1
            lines.append(f"- [{source}:{index}] {item}")
        lines.append("")

    lines.append(f"Total checklist bullets: {total}")
    output_path.write_text("\n".join(lines) + "\n", encoding="utf-8")

    summary = {
        "checklist_path": str(output_path),
        "sources": {source: len(items) for source, items in sorted(sources.items())},
        "total": total,
    }
    print(json.dumps(summary, indent=2))


if __name__ == "__main__":
    main()
