import io
import os
import shutil
import sys
import tarfile
import urllib.error
import urllib.request
import zipfile
from enum import Enum

# Colors are disabled in non-TTY environments such as pipes. This means
# that if output is redirected to a file, it won't contain color codes.
# Colors are always enabled on continuous integration.
_colorize = bool(sys.stdout.isatty() or os.environ.get("CI"))


class ANSI(Enum):
    """
    Enum class for adding ansi colorcodes directly into strings.
    Automatically converts values to strings representing their
    internal value, or an empty string in a non-colorized scope.
    """

    RESET = "\x1b[0m"

    BOLD = "\x1b[1m"
    ITALIC = "\x1b[3m"
    UNDERLINE = "\x1b[4m"
    STRIKETHROUGH = "\x1b[9m"
    REGULAR = "\x1b[22;23;24;29m"

    BLACK = "\x1b[30m"
    RED = "\x1b[31m"
    GREEN = "\x1b[32m"
    YELLOW = "\x1b[33m"
    BLUE = "\x1b[34m"
    MAGENTA = "\x1b[35m"
    CYAN = "\x1b[36m"
    WHITE = "\x1b[37m"

    PURPLE = "\x1b[38;5;93m"
    PINK = "\x1b[38;5;206m"
    ORANGE = "\x1b[38;5;214m"
    GRAY = "\x1b[38;5;244m"

    def __str__(self) -> str:
        global _colorize
        return str(self.value) if _colorize else ""


def print_warning(*values: object) -> None:
    """Prints a warning message with formatting."""
    print(f"{ANSI.YELLOW}{ANSI.BOLD}WARNING:{ANSI.REGULAR}", *values, ANSI.RESET, file=sys.stderr)


def print_error(*values: object) -> None:
    """Prints an error message with formatting."""
    print(f"{ANSI.RED}{ANSI.BOLD}ERROR:{ANSI.REGULAR}", *values, ANSI.RESET, file=sys.stderr)


def read_addon_stamp(path: str) -> str | None:
    """Returns the version recorded in an addon stamp file, or None."""
    try:
        with open(path) as f:
            tokens = f.read().split()
        return tokens[-1] if tokens else None
    except OSError:
        return None


def _iter_archive_files(payload: bytes):
    """Yields (root, relative_path, fileobj) for regular files in a tar or zip.

    Format is detected by magic bytes: archives from GitHub arrive as tarballs,
    Bitbucket commit snapshots as zips.
    """
    if payload[:2] == b"PK":
        with zipfile.ZipFile(io.BytesIO(payload)) as zf:
            for info in zf.infolist():
                if info.is_dir():
                    continue
                parts = info.filename.split("/", 1)
                if len(parts) != 2:
                    continue
                yield parts[0], parts[1], zf.open(info)
    else:
        with tarfile.open(fileobj=io.BytesIO(payload), mode="r:*") as tf:
            for member in tf.getmembers():
                if not member.isfile():
                    continue
                parts = member.name.split("/", 1)
                if len(parts) != 2:
                    continue
                yield parts[0], parts[1], tf.extractfile(member)


def fetch_addon(target, source, env) -> None:
    """SCons action: download an addon archive and extract its subtree.

    Reads addon_name, addon_version, addon_url, addon_subdir, addon_dest from
    the construction environment. On success writes the stamp file target[0].
    """
    name = env["addon_name"]
    version = env["addon_version"]
    url = env["addon_url"]
    subdir = env["addon_subdir"].strip("/")
    dest = env["addon_dest"]
    stamp = str(target[0])

    print(f"Fetching {name} {version} ...")
    try:
        with urllib.request.urlopen(url) as resp:
            payload = resp.read()
    except urllib.error.URLError as e:
        print_error(f"failed to download {url}: {e}")
        sys.exit(1)

    shutil.rmtree(dest, ignore_errors=True)
    os.makedirs(dest, exist_ok=True)

    # subdir is relative to the archive's root component (which the forge names
    # after the repo and its tag or commit).
    extracted = 0
    roots = set()
    for root, rel, src in _iter_archive_files(payload):
        if not rel.startswith(subdir + "/"):
            continue
        roots.add(root)
        out = os.path.join(dest, rel[len(subdir) + 1:])
        os.makedirs(os.path.dirname(out), exist_ok=True)
        if src is None:
            continue
        with open(out, "wb") as dstf:
            shutil.copyfileobj(src, dstf)
        extracted += 1

    if len(roots) > 1:
        print_error(f"ambiguous matches for '{subdir}' under multiple archive roots: {sorted(roots)}")
        shutil.rmtree(dest, ignore_errors=True)
        sys.exit(1)
    if extracted == 0:
        print_error(f"no files found under '{subdir}' in {url}")
        shutil.rmtree(dest, ignore_errors=True)
        sys.exit(1)

    open(stamp, "w").write(f"{version}\n")
