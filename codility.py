#lesson 1
def solution(N):
    bstr = dectoBin(N)
    arr = []
    cnt = 0
    for b in bstr:
        if b == '0':
            cnt = cnt + 1
        if b != '0':
            arr.append(cnt)
            cnt = 0 
    return getMax(arr)

def dectoBin(N):
    bstr = ""
    while N > 0:
        bstr = str(N % 2) + bstr
        N = N // 2
    return bstr

def getMax(arr):
    max = arr[0]
    for i in range(len(arr)):
        if arr[i] > max:
            max = arr[i]
    return max

solution(0)

def bestSolution(arr):
    r = 0
    for i in range(len(arr)):
        r = r ^ arr[i]
    return r

#solution2([9, 3, 9, 3, 9, 7, 9])
bestSolution([9, 3, 9, 3, 9, 7, 9])
