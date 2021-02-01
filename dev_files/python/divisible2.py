def f(n):
    a = ((11**(n)) - (2**(n)))//9
    print("a=", str(a))
    if n >= 10:
        b = a - ( ((11**(n-10)) * n) + (n * f(n-10)) )
        if n >= 20:
            b = b - ( ((11**(n-20)) * n) + (n * f(n-20)) )
        return b
    else:
        return a
n = 21
print(f(n))
print("---")
print(f(n-10))