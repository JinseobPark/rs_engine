"""
Python script to update Doxyfile version using source code

:Author: jupiterbjy@gmail.com
"""

import pathlib
import re


# --- Config ---

ROOT = pathlib.Path.cwd()
TARGET_SRC = ROOT / "src" / "RSEngine.h"
DOXY = ROOT / "Doxyfile"


# Version matching regex. Replace via $1VERSION$3
VERSION_REGEX = re.compile(
    r'(PROJECT_NUMBER\s+=\s+)"(.*)"'
)

# Project name matching regex. Replace via $1PROJECT_NAME$3
PROJECT_NAME_REGEX = re.compile(
    r'(PROJECT_NAME\s+=\s+")(.*)(")'
)


# --- Config assert ---

assert TARGET_SRC.exists(), f"Target source file not found: {TARGET_SRC}"
assert DOXY.exists(), "Doxygen configuration file not found"


# --- Utilities ---

def replace_version(content: str, version: str) -> str:
    """Replace version in given content.

    Args:
        content: content to replace version.
        version: version string.

    Returns:
        content with version replaced.
    """

    return VERSION_REGEX.sub(f'\\1"{version}"', content)


# --- Logics ---

def fetch_version(path: pathlib.Path) -> str:
    """Fetch version from given file path.

    Args:
        path: path to doxygen documented file.

    Returns:
        version string
    """

    for line in path.read_text().splitlines():
        if "@version" in line:
            return line.split()[-1]

    return "VERSION_UNKNOWN"


def update_doxy(doxy_path: pathlib.Path, version: str) -> None:
    """Replace version in given file path.

    Args:
        doxy_path: path to doxygen configuration file.
        version: version string.
    """

    doxy_content = doxy_path.read_text()
    doxy_content = replace_version(doxy_content, version)

    print("[UpdateDoxyVersion] Doxyfile updated with version:", version)

    doxy_path.write_text(doxy_content)


if __name__ == '__main__':
    update_doxy(DOXY, fetch_version(TARGET_SRC))
