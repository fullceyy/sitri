#!/usr/bin/env python3
"""Copy build/firmware/compile_commands.json to the project root,
rewritten with \\wsl.localhost\... paths so Windows clangd understands it."""
import json, os, re, shlex
from pathlib import Path

root = Path(__file__).resolve().parent.parent
src = root / "build" / "firmware" / "compile_commands.json"
distro = os.environ.get("WSL_DISTRO_NAME", "Ubuntu")

def win(path: str) -> str:
    return "W:" + distro + path.replace("/", "\\")

FLAG_WITH_PATH = re.compile(r"^(-I|-isystem|-iquote|-include|-o)(/.*)$")

def convert(arg: str) -> str:
    if arg.startswith("/"):
        return win(arg)
    m = FLAG_WITH_PATH.match(arg)
    return m.group(1) + win(m.group(2)) if m else arg

entries = json.loads(src.read_text())
for e in entries:
    args = e.pop("arguments", None) or shlex.split(e.pop("command"))
    driver = os.path.basename(args[0])          # e.g. arm-none-eabi-gcc
    e["arguments"] = (
        [driver]
        + [convert(a) for a in args[1:]]
        + ["--target=arm-none-eabi",
           "-isystem", win("/usr/lib/arm-none-eabi/include")]  # newlib headers
    )
    e["directory"] = win(e["directory"])
    e["file"] = win(e["file"])

(root / "compile_commands.json").write_text(json.dumps(entries, indent=2))
print(f"Wrote {len(entries)} entries for distro '{distro}'")