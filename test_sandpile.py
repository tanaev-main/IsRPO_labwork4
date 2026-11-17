"""Unit-тесты для C++-программы песчаной модели."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


BRIDGE = r"""
// Оригинальная точка входа не вызывается из тестов.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main sandpile_program_main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 2;
    }

    std::uint64_t field[] = {0, 1, 2, static_cast<std::uint64_t>(argv[1][0] - '0')};
    std::cout << std::boolalpha << IsStable(field, 2, 2) << '\n';
}
"""


class SandpileStabilityTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        source_dir = Path(__file__).resolve().parent
        if not (source_dir / "main.cpp").is_file():
            raise FileNotFoundError("Положите test_sandpile.py рядом с main.cpp.")

        build_dir = tempfile.TemporaryDirectory()
        cls.addClassCleanup(build_dir.cleanup)
        bridge_path = Path(build_dir.name) / "test_bridge.cpp"
        bridge_path.write_text(BRIDGE, encoding="utf-8")
        executable = "test_bridge.exe" if os.name == "nt" else "test_bridge"
        cls.executable = Path(build_dir.name) / executable
        compiler = os.environ.get("CXX", "g++" if os.name == "nt" else "c++")
        subprocess.run(
            [
                compiler,
                "-std=c++17",
                "-I",
                str(source_dir),
                str(bridge_path),
                "-o",
                str(cls.executable),
            ],
            check=True,
        )

    def run_stability_check(self, last_cell):
        result = subprocess.run(
            [str(self.executable), str(last_cell)],
            check=True,
            capture_output=True,
            text=True,
        )
        return result.stdout.strip()

    def test_stable_when_every_cell_has_at_most_three_grains(self):
        self.assertEqual(self.run_stability_check(3), "true")

    def test_unstable_when_last_cell_has_four_grains(self):
        self.assertEqual(self.run_stability_check(4), "false")


if __name__ == "__main__":
    unittest.main()
