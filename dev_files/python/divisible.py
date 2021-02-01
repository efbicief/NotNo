def f(n):
    a = ((11**(n)) - (2**(n)))//9
    if n >= 10:
        a = a - (((11**(n-10)) * n) + (n * f(n-10)))
    return a
for i in range(0,20):
    print(str(i) + ": " + str(f(i)))