import numpy as np
import seaborn as sns
import matplotlib.pylab as plt


def change(c) -> int:
    if c == '*':
        return 0
    return int(c)


testcase = "tests/03.txt"
with open(testcase, "r") as file:
    lines = file.readlines()

    grid = [list(map(change, line.split())) for line in lines[2:]]
    # print(grid)
    grid = np.array(grid)
    # print(grid.shape)
    # for col in range(300, 1000):
    #     c = [grid[row][col] for row in range(grid.shape[0])]
    #     if len(set(c)) == 1:
    #         print(col, grid[0][col])

    ax = sns.heatmap(grid)
    plt.show()
    # plt.savefig("plot.png")


# uniform_data = np.random.rand(10, 12)
