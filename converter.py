w, h, s = map(int, input().split())

input()
for row in range(h):
    print(''.join('#' if val == '*' or int(val) >= 0 else '.' for val in input().split()))