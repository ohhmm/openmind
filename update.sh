#!/bin/bash
if ! command -v python3 >/dev/null || ! python3 -c 'pass' 2>/dev/null; then
    printf '%s\n' 'Automatic update recovery requires Python 3. Running the standard Git update.' >&2
    exec git -C "$(dirname -- "$0")" pull --rebase --autostash origin main
fi
exec python3 -c 'import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

os.chdir(Path(sys.argv[1]).parent.resolve())
root = Path.cwd()
pull = ("pull", "--rebase", "--autostash", "origin", "main")
collision = re.compile(
    rb"^error: The following untracked working tree files would be overwritten by (checkout|merge):$",
    re.MULTILINE,
)
backups = []
scratch = None


def git(*args, **options):
    return subprocess.run(["git", *args], **options)


def output(*args):
    return git(*args, stdout=subprocess.PIPE, check=True).stdout


def paths(*args):
    return output(*args).rstrip(b"\0").split(b"\0")


def succeeds(*args):
    return git("--literal-pathspecs", *args, stdout=subprocess.DEVNULL,
               stderr=subprocess.DEVNULL).returncode == 0


def safe_parent(file):
    return file.parent.resolve() == root / file.parent


def save(original, target):
    global scratch
    if not safe_parent(original):
        raise ValueError(f"Unsafe path for backup: {original}")
    if scratch is None:
        scratch = Path(tempfile.mkdtemp())
    saved = scratch / str(len(backups))
    shutil.move(original, saved)
    backups.append((original, target, saved))


def protect_autostash():
    # Autostash can recreate a tracked directory before Git checks collisions.
    for entry in paths("diff", "--name-only", "--diff-filter=D", "-z", "HEAD"):
        for parent in reversed(Path(os.fsdecode(entry)).parents):
            if parent.is_symlink() or parent.is_file():
                if not succeeds("cat-file", "-e", f":{parent}"):
                    save(parent, parent)
                break


def back_up_collisions():
    mode = "icase,literal" if succeeds("config", "--get-regexp", r"^core\.ignorecase$", r"^true$") else "literal"
    changes = iter(paths("diff", "--raw", "--no-renames", "--diff-filter=A", "-z", "HEAD", "FETCH_HEAD"))
    for change, name in zip(changes, changes):
        target = Path(os.fsdecode(name))
        if not change.startswith((b":000000 100644 ", b":000000 100755 ")):
            continue
        if target.is_symlink() or not target.is_file():
            continue
        matches = paths("ls-files", "--others", "--exclude-standard", "-z", "--", f":({mode}){target}")
        if len(matches) == 1 and matches[0]:
            original = Path(os.fsdecode(matches[0]))
            if not original.is_symlink() and original.samefile(target):
                save(original, target)


def restore(status):
    result = status
    for original, target, saved in backups:
        file = original if status else target
        try:
            if file.is_symlink() or not safe_parent(file):
                raise ValueError("unsafe destination")
            if file.exists() and (status or not file.is_file() or
                                  not succeeds("ls-files", "--error-unmatch", "--", str(file)) or
                                  not succeeds("diff", "--quiet", "HEAD", "--", str(file))):
                raise ValueError("destination contains another version")
            shutil.move(saved, file)
            if not status and not succeeds("diff", "--quiet", "--", str(file)):
                print(f"Warning: restored differing local copy of {file}. Review it with git diff.", file=sys.stderr)
        except (OSError, ValueError):
            print(f"Preserved local {original} at {saved}", file=sys.stderr)
            result = result or 1
    if scratch is not None and not any(scratch.iterdir()):
        scratch.rmdir()
    return result


def update():
    start = output("rev-parse", "HEAD")
    protect_autostash()
    first = git(*pull, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                env=dict(os.environ, LC_ALL="C"))
    sys.stdout.buffer.write(first.stdout)
    sys.stdout.flush()
    if not first.returncode or not collision.search(first.stdout):
        return first.returncode
    if output("rev-parse", "HEAD") != start or any(
        Path(os.fsdecode(output("rev-parse", "--git-path", state).rstrip())).is_dir()
        for state in ("rebase-merge", "rebase-apply")
    ):
        return first.returncode

    before = len(backups)
    back_up_collisions()
    if len(backups) == before:
        return first.returncode
    print(f"Retrying with {len(backups)} local file(s) backed up.", file=sys.stderr)
    return git(*pull).returncode


status = 1
try:
    status = update()
except (OSError, ValueError, subprocess.CalledProcessError) as error:
    print(f"Update failed: {error}", file=sys.stderr)
except KeyboardInterrupt:
    status = 130
finally:
    status = restore(status)
sys.exit(128 - status if status < 0 else status)
' "$0"
