# 基于MPI和OpenMP求解拉普拉斯方程
理论说明请参考[解题报告](拉普拉斯方程解题报告.pdf)。

## 如何使用
代码路径：/public/home/test/LiuDongjing/src

查看帮助
```
make help
```

编译并行化程序并运行
```
make
```

编译串行化程序并运行
```
make MODE=SERIAL
```

运行指定内部节点个数(下面的内部节点是100×100)
```
make run 100
```
