import ast
from pathlib import Path
import re
import sys


# ponytail: No Python relative library imports or leading solution future import,
# Rust super:: cross-module references, or non-cplib local C++ includes.
def bundle_cpp(source: str, root: Path) -> str:
    seen: set[Path] = set()

    def expand(text: str) -> str:
        def include(match: re.Match[str]) -> str:
            path = (root / "cpp" / match[1]).resolve()
            if path in seen:
                return ""
            seen.add(path)
            body = re.sub(r"^\s*#pragma once[^\n]*$", "", path.read_text(), flags=re.M)
            return expand(body) + "\n"

        return re.sub(r'^\s*#\s*include\s*[<"](cplib/[^>"\n]+)[>"][^\n]*$',
                      include, text, flags=re.M)

    return expand(source)


def bundle_python(source: str, root: Path) -> str:
    modules: dict[str, tuple[bool, str]] = {}

    def locate(name: str) -> tuple[Path, bool]:
        path = root / "python" / name.replace(".", "/")
        if (path / "__init__.py").is_file():
            return path / "__init__.py", True
        return path.with_suffix(".py"), False

    def add(name: str) -> None:
        if name in modules:
            return
        path, package = locate(name)
        body = path.read_text()
        modules[name] = package, body
        if "." in name:
            add(name.rsplit(".", 1)[0])
        scan(body)

    def scan(text: str) -> None:
        for node in ast.walk(ast.parse(text)):
            if isinstance(node, ast.Import):
                for alias in node.names:
                    if alias.name == "cplib" or alias.name.startswith("cplib."):
                        add(alias.name)
            elif isinstance(node, ast.ImportFrom) and not node.level:
                name = node.module or ""
                if name == "cplib" or name.startswith("cplib."):
                    add(name)
                    for alias in node.names:
                        child = name + "." + alias.name
                        if locate(child)[0].is_file():
                            add(child)

    scan(source)
    return """import importlib.abc as _bundle_abc
import importlib.util as _bundle_util
import sys as _bundle_sys

_bundle_sources = """ + repr(modules) + """

class _BundleLoader(_bundle_abc.MetaPathFinder, _bundle_abc.Loader):
    def find_spec(self, fullname, path=None, target=None):
        if fullname in _bundle_sources:
            return _bundle_util.spec_from_loader(
                fullname, self, is_package=_bundle_sources[fullname][0])

    def create_module(self, spec):
        return None

    def exec_module(self, module):
        exec(compile(_bundle_sources[module.__name__][1],
                     '<bundled ' + module.__name__ + '>', 'exec'), module.__dict__)

_bundle_sys.meta_path.insert(0, _BundleLoader())

""" + source


def bundle_rust(source: str, root: Path) -> str:
    declaration = re.compile(r"\b((?:pub(?:\s*\([^)]*\))?\s+)?mod\s+)(\w+)\s*;")
    library = (root / "rust/src/lib.rs").read_text()
    names = {match[2] for match in declaration.finditer(library)}

    def references(text: str, prefix: str) -> set[str]:
        found: set[str] = set()
        # A group contains top-level use paths; nested groups belong to its first name.
        for match in re.finditer(r"\b" + prefix + r"\s*::\s*", text):
            tail = text[match.end():]
            if tail.startswith("*"):
                found.update(names)
            elif tail.startswith("{"):
                depth = 0
                start = 1
                for i, char in enumerate(tail):
                    if char == "{":
                        depth += 1
                    elif char == "}":
                        depth -= 1
                    if (char == "," and depth == 1) or depth == 0:
                        item = tail[start:i].strip()
                        if item == "*":
                            found.update(names)
                        else:
                            first = re.match(r"\w+", item)
                            if first:
                                found.add(first[0])
                        start = i + 1
                    if depth == 0:
                        break
            else:
                first = re.match(r"\w+", tail)
                if first:
                    found.add(first[0])
        return found & names

    def expand(text: str, directory: Path) -> str:
        def module(match: re.Match[str]) -> str:
            name = match[2]
            path = directory / (name + ".rs")
            if not path.is_file():
                path = directory / name / "mod.rs"
            body = expand(path.read_text(), directory / name)
            return match[1] + name + " {\n" + body + "\n}"

        return declaration.sub(module, text)

    needed = references(source, "cplib") | references(library, "crate")
    expanded: dict[str, str] = {}
    while needed - expanded.keys():
        name = sorted(needed - expanded.keys())[0]
        body = expand("pub mod " + name + ";", root / "rust/src")
        expanded[name] = body[body.index("{") + 1:body.rindex("}")]
        needed.update(references(body, "crate"))

    def top_module(match: re.Match[str]) -> str:
        name = match[2]
        if name not in needed:
            return ""
        return match[1] + name + " {" + expanded[name] + "}"

    body = declaration.sub(top_module, library)
    body = re.sub(r"\bcrate\s*::", "crate::cplib::", body)
    return source + "\n\npub mod cplib {\n" + body + "\n}\n"


def main() -> None:
    if len(sys.argv) != 2:
        sys.exit("usage: python3 tools/bundle.py <solution file>")
    path = Path(sys.argv[1])
    bundlers = {".cpp": bundle_cpp, ".py": bundle_python, ".rs": bundle_rust}
    if path.suffix not in bundlers:
        sys.exit("unsupported solution extension: " + path.suffix)
    try:
        result = bundlers[path.suffix](path.read_text(), Path(__file__).resolve().parents[1])
    except (OSError, SyntaxError) as error:
        sys.exit(str(error))
    sys.stdout.write(result)


if __name__ == "__main__":
    main()
