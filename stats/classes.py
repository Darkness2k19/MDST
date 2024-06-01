from dataclasses import dataclass


@dataclass
class Edge:
    v_from: int
    v_to: int

    def to_str(self):
        return f"{self.v_from} {self.v_to}"


@dataclass
class TestCase:
    vert_cnt: int
    edges_cnt: int
    edges: list[Edge]

    def to_str(self):
        return f"{self.vert_cnt} {self.edges_cnt}\n" + \
            ' '.join([str(i) for i in range(1, self.vert_cnt+1)]) + '\n' + \
            '\n'.join([i.to_str() for i in self.edges])
