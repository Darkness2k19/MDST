import os
import json
import shutil
import random
import time

from subprocess import PIPE, run

from generator import Generator
from classes import TestCase, Edge
from draw_plot import draw_plot


TIMEOUT = 30


class Validator:
    def __init__(self, config_file: str = 'stats/tests_config.json'):
        with open(config_file, 'r') as f:
            self.config = json.load(f)['test_groups']
        self.tests: dict[str, list[TestCase]] = dict()

    def clean_environment(self):
        for dir in ['build', 'bin']:
            try:
                shutil.rmtree(dir)
            except:
                pass

    def build_binaries(self):
        COMMANDS = {'solver': 'cmake .. -DDEBUG=OFF -DDUMMY=OFF -DSTATS=ON -DTESTING=OFF',
                    'checker': 'cmake .. -DDEBUG=OFF -DDUMMY=ON -DSTATS=ON -DTESTING=OFF'}

        try:
            shutil.rmtree('binaries')
        except:
            pass
        os.mkdir('binaries')

        for name, command in COMMANDS.items():
            self.clean_environment()
            os.system(f'mkdir build && cd build && {command} && make && cd ../')
            shutil.copyfile('bin/main', f'binaries/{name}')
            os.system(f'chmod +x binaries/{name}')
        self.clean_environment()

    def generate_tests(self):
        for group in self.config:
            name = group['name']
            self.tests[name] = []
            regen_factor = group['regen_factor']
            type = group['type']
            for param_gr in group['parameters']:
                generator = Generator(vertex_count=param_gr['vertex_count'], edges_count=param_gr['edges_count'], seed=time.time_ns())
                for _ in range(regen_factor):
                    if type == 'usual':
                        self.tests[name].append(generator.generate())
                    elif type == 'components':
                        part1 = generator.generate()
                        part2 = generator.generate()
                        addition = part1.vert_cnt
                        for i in range(part2.edges_cnt):
                            part2.edges[i] = Edge(part2.edges[i].v_from+addition, part2.edges[i].v_to+addition)

                        new_edge = Edge(random.randint(1, part1.vert_cnt), random.randint(part1.vert_cnt+1, part1.vert_cnt + part2.vert_cnt))
                        test = TestCase(part1.vert_cnt+part2.vert_cnt, part1.edges_cnt + part2.edges_cnt + 1, part1.edges + [new_edge] + part2.edges)
                        self.tests[name].append(test)
                    else:
                        raise ValueError("Unsupported test type passed!")

    def dfs(self, vert, verts, edges, used):
        if used[vert]:
            return
        used[vert] = True

        for e in edges:
            x, y = e
            if x == vert:
                self.dfs(y, verts, edges, used)
            elif y == vert:
                self.dfs(x, verts, edges, used)

    def check_connectivity(self, verts, edges):
        used = {i: False for i in verts}
        self.dfs(verts[0], verts, edges, used)

        return all(list(used.values()))

    def validate_test_output(self, solver_result: str, checker_result: str, test: TestCase) -> int:
        # 1 - OK, 0 - Error, -1 - OK, but not best

        solver_result = solver_result.strip()
        checker_result = checker_result.strip()

        # step 1: check that we don't have errors
        if checker_result == 'no mst in graph':
            assert solver_result == 'no mst in graph', 'solver not agree with no mst. :)'
            return 1

        solver_lines = solver_result.split('\n')
        checker_lines = checker_result.split('\n')

        # step 2: check parameters
        s_v_cnt, s_e_cnt, s_deg = map(int, solver_lines.pop(0).split())
        c_v_cnt, c_e_cnt, c_deg = map(int, checker_lines.pop(0).split())

        assert test.vert_cnt == s_v_cnt == c_v_cnt, "vertex count mismatch!"
        assert test.vert_cnt - 1 == s_e_cnt == c_e_cnt, "edges count mismatch (not v_cnt - 1)!"

        assert s_deg in [c_deg, c_deg+1], "solver deg is not in [checker_deg, checker_deg + 1]!"
        if s_deg == c_deg:
            res = 1
        else:
            res = -1

        solver_verts = sorted(map(int, solver_lines.pop(0).split()))
        checker_verts = sorted(map(int, checker_lines.pop(0).split()))
        assert solver_verts == checker_verts, 'solver and checker returned different sets of verts!'

        solver_edges = []
        for i in solver_lines:
            x, y = i.split()
            solver_edges.append((int(x), int(y)))

        checker_edges = []
        for i in solver_lines:
            x, y = i.split()
            checker_edges.append((int(x), int(y)))

        assert self.check_connectivity(solver_verts, solver_edges), "solver connectivity check failed!"
        assert self.check_connectivity(checker_verts, checker_edges), "checker connectivity check failed!"

        return res

    def run_test(self, test: TestCase) -> int:
        try:
            input_str = test.to_str()

            solver_command = ['./binaries/solver']
            solver_out = run(solver_command, input=input_str, stdout=PIPE, stderr=PIPE, universal_newlines=True, timeout=TIMEOUT)
            assert not solver_out.stderr, f'solver stderr not empty: {solver_out.stderr}'
            solver_result = solver_out.stdout

            checher_command = ['./binaries/checker']
            checker_out = run(checher_command, input=input_str, stdout=PIPE, stderr=PIPE, universal_newlines=True, timeout=TIMEOUT)
            assert not checker_out.stderr, f'checker stderr not empty: {checker_out.stderr}'
            checker_result = checker_out.stdout

            return self.validate_test_output(solver_result, checker_result, test)
        except Exception as err:
            print("Error:", err)
            return 0

    def run_all_tests(self, ignored_groups = []) -> dict[str, list[int]]:
        stats = {}
        for gr, tests in self.tests.items():
            print(f"Running test group '{gr}'...")
            errors_cnt = 0
            tests_cnt = 0
            for test in tests:
                tests_cnt += 1
                res = self.run_test(test)
                if res == 0:
                    print(f"FAILED test #{tests_cnt}:")
                    print(test.to_str())
                    print("Exit...")
                    exit(0)

                elif res == -1:
                    errors_cnt += 1

            print(f"{tests_cnt} / {len(tests)} tests OK!")

            if gr in ignored_groups:
                continue

            stats[gr] = [tests_cnt - errors_cnt, tests_cnt]
        print("All tests passed!")

        return stats

    def build_and_run(self):
        self.build_binaries()
        self.generate_tests()
        return self.run_all_tests(ignored_groups=["Несвязные графы"])


if __name__ == "__main__":
    v = Validator()
    stats = v.build_and_run()
    stats = {x: round(y[0] / y[1], 3) for x, y in stats.items()}
    draw_plot(stats)
