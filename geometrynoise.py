n=50;m=50
def noise(i,j):
    return i*j/2500*256
h=[]
for i in range(0,n):
    h.append([])
    for j in range(0,m):
        h[i].append(noise(i,j))
from tkinter import *
rt=Tk()
rt.geometry(str(n*10)+"x"+str(m*10)+"+500+100")
w=Canvas(rt)
w.pack(fill=BOTH,expand=True)
def getclr(ht):
    return "#"+("00"+str(hex(ht))[2:])[-2:]+"7f"+("00"+str(hex(255-ht))[2:])[-2:]
print(0,getclr(0))
print(50,getclr(50))
print(100,getclr(100))
print(150,getclr(150))
print(200,getclr(200))
print(250,getclr(250))
print(noise(25,25),h[25][25])
for i in range(0,n):
    for j in range(0,m):
        ht=round(h[i][j])
        w.create_rectangle(i*10,j*10,i*10+10,j*10+10,fill=getclr(ht),width=0)
