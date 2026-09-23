import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

from bundle import bundle_cpp, bundle_python, bundle_rust


ROOT = Path(__file__).resolve().parents[1]


def write(root: Path, name: str, source: str) -> None:
    path = root / name
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(source)


def run(source: str, suffix: str, directory: Path) -> None:
    tool = {".cpp": "g++", ".py": "python3", ".rs": "rustc"}[suffix]
    executable = shutil.which(tool)
    if executable is None:
        print(f"SKIP {suffix} compile/run: {tool} missing")
        return
    path = directory / ("main" + suffix)
    path.write_text(source)
    binary = directory / "main"
    if suffix == ".cpp":
        subprocess.run([executable, "-std=gnu++20", str(path), "-o", str(binary)],
                       check=True, cwd=directory)
    elif suffix == ".rs":
        subprocess.run([executable, str(path), "-o", str(binary)],
                       check=True, cwd=directory)
    env = dict(os.environ, PYTHONPATH="")
    command = [executable, str(path)] if suffix == ".py" else [str(binary)]
    result = subprocess.run(command, check=True, capture_output=True, text=True,
                            cwd=directory, env=env)
    assert result.stdout == "-4 -3\n", result.stdout
    print(f"PASS {suffix} compile/run")


def main() -> None:
    with tempfile.TemporaryDirectory() as temporary:
        root = Path(temporary) / "library"
        work = Path(temporary) / "run"
        work.mkdir()
        write(root, "cpp/cplib/used.hpp", """#pragma once
inline int used_marker() { return -4; }
""")
        write(root, "cpp/cplib/div.hpp", """#pragma once
#include "cplib/used.hpp"
inline int div_marker() { return used_marker(); }
""")
        write(root, "cpp/cplib/unused.hpp", "int unused_marker;\n")
        cpp = """#include <iostream>
#include <cplib/div.hpp>
#include "cplib/used.hpp"
int main() { std::cout << div_marker() << " -3\\n"; }
"""
        write(root, "python/cplib/__init__.py", "package_marker = True\n")
        write(root, "python/cplib/used.py", "def used_marker(): return -4\n")
        write(root, "python/cplib/div.py", """from cplib import used
def div_marker(): return used.used_marker()
""")
        write(root, "python/cplib/unused.py", "unused_marker = True\n")
        python = """import cplib.div
from cplib.div import div_marker
print(div_marker(), -3)
"""
        write(root, "rust/src/lib.rs", "pub mod div;\npub mod used;\npub mod unused;\n")
        write(root, "rust/src/div.rs", """mod nested;
pub fn div_marker() -> i32 { nested::value() }
""")
        write(root, "rust/src/div/nested.rs",
              "pub fn value() -> i32 { crate::used::used_marker() }\n")
        write(root, "rust/src/used/mod.rs", "pub fn used_marker() -> i32 { -4 }\n")
        write(root, "rust/src/unused.rs", "pub fn unused_marker() {}\n")
        rust = 'fn main() { println!("{} -3", cplib::div::div_marker()); }\n'
        fixtures = [(".cpp", bundle_cpp, cpp), (".py", bundle_python, python),
                    (".rs", bundle_rust, rust)]
        for suffix, bundler, source in fixtures:
            output = bundler(source, root)
            assert "div_marker" in output and "used_marker" in output
            assert "unused_marker" not in output
            if suffix == ".cpp":
                assert "#pragma once" not in output
                assert output.count("inline int used_marker") == 1
                assert "#include <iostream>" in output
            if suffix == ".py":
                assert "package_marker" in output
            print(f"PASS {suffix} dependency closure")
            run(output, suffix, work)

        for reference in ["use cplib::{div, used::used_marker};",
                          "use cplib::{div::{div_marker}, used};",
                          "use cplib::*;"]:
            output = bundle_rust(reference, root)
            assert "div_marker" in output and "used_marker" in output
            assert ("unused_marker" in output) == ("cplib::*" in reference)
        assert "crate::cplib::used::" in bundle_rust(rust, root)
        assert "unused_marker" not in bundle_rust("fn main() {}", root)
        write(root, "python/cplib/nested/__init__.py", "nested_marker = True\n")
        write(root, "python/cplib/nested/value.py", "from cplib.div import div_marker\n")
        nested = bundle_python("from cplib.nested import value\n"
                               "print(value.div_marker(), -3)\n", root)
        assert "nested_marker" in nested and "unused_marker" not in nested
        run(nested, ".py", work)

        real = {
            ".cpp": """#include <iostream>
#include <cplib/div.hpp>
int main() { std::cout << cplib::floor_div(-7, 2) << ' '
                      << cplib::ceil_div(-7, 2) << '\\n'; }
""",
            ".py": "from cplib.div import floor_div, ceil_div\n"
                   "print(floor_div(-7, 2), ceil_div(-7, 2))\n",
            ".rs": 'use cplib::div::{floor_div, ceil_div};\n'
                   'fn main() { println!("{} {}", floor_div(-7, 2), ceil_div(-7, 2)); }\n',
        }
        for suffix, source in real.items():
            path = work / ("solution" + suffix)
            path.write_text(source)
            result = subprocess.run([sys.executable, str(ROOT / "tools/bundle.py"), str(path)],
                                    check=True, capture_output=True, text=True, cwd=work)
            assert "floor_div" in result.stdout and "ceil_div" in result.stdout
            print(f"PASS {suffix} real library CLI smoke")
            run(result.stdout, suffix, work)
        result = subprocess.run([sys.executable, str(ROOT / "tools/bundle.py"), "bad.txt"],
                                capture_output=True, text=True, cwd=work)
        assert result.returncode != 0 and "extension" in result.stderr
        assert not result.stdout
        print("PASS unsupported extension")


if __name__ == "__main__":
    main()
