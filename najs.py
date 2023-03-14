w, h, s = map(int, input().split())

size = w * h
sum_s = sum(map(int, input().split()))
cell_values = []
cell_values_non_zero = []
for row in range(h):
    inp = input()
    cell_values.extend([0 if x == '*' else int(x) for x in inp.split()])
    cell_values_non_zero.extend([0 if x == '*' else max(int(x), 0) for x in inp.split()])
cell_values.sort(key=lambda x: -x)
cell_values_non_zero.sort(key=lambda x: -x)

print(f'Max score: {sum(cell_values[:sum_s])}')
print(f'Max score non zero: {sum(cell_values_non_zero[:sum_s])}')
print(f"{size=}, {sum_s=}, {size / sum_s=}")