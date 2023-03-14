import argparse
import subprocess
import os


def main():
    """
    It requires <prog> output to be:
    stdout: <testcase> output
    stderr: Score: <score>
    """

    parser = argparse.ArgumentParser()
    parser.add_argument("prog", type=str)
    parser.add_argument("testcase", type=str)
    parser.add_argument("--threads", "-t", type=int, default=1)

    args = parser.parse_args()

    print(args)

    prog = args.prog
    testcase = args.testcase
    threads = args.threads

    processes: list[subprocess.Popen] = []

    for _ in range(threads):
        file = open(testcase, "r")
        process = subprocess.Popen(
            prog,
            stdin=file,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True)

        processes.append(process)

    print("Waiting for processes")

    best: None | tuple[float, str] = None

    for process in processes:
        stdout, stderr = process.communicate()
        score = stderr.decode()
        if "Score: " not in score:
            print("Invalid stderr! No Score: in stderr.")

        score = float(score.rsplit("Score: ", 1)[1])
        print(score)

        if best is None or best[0] < score:
            best = (score, stdout.decode())

    print("Best score: ", best[0])

    base_name = os.path.basename(testcase)
    base_name = os.path.splitext(base_name)[0]

    with open(f"outs/{base_name}.{str(best[0])}.txt", "w") as file:
        file.write(best[1])


if __name__ == "__main__":
    main()
