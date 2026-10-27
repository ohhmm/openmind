#!/usr/bin/env python3
"""Collect pre-push QA checklist bullets from AGENTS.md/CLAUDE.md and referenced Markdown."""

import json
import re
from pathlib import Path

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
