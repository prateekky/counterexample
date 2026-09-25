n = 100_000
pattern_index = 50_000

values = [4] * n
values[pattern_index] = 4
values[pattern_index + 1] = 9
values[pattern_index + 2] = 0
values[pattern_index + 3] = 9

with open("large.txt", "w") as f:
    f.write(f"{n}\n")
    f.write(" ".join(map(str, values)))
    f.write("\n")