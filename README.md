直接开放吧，有兴趣的话可以编译玩玩

https://www.bilibili.com/video/BV1qCBBYkEXS

硬件：https://oshwhub.com/eedadada/phoebe-4-real

---



- 新的 Mooncake 项目框架
- 带 UI、HAL API 绑定的 JS runtime，可用于表盘、或者 app
- 更好用的 Lvgl cpp 绑定，以及过渡动画支持



## 桌面端编译

拉取依赖：

```bash
python fetch_repos.py
```

编译：

```bash
mkdir build && cd build
```

```
cmake .. && make
```

运行：

```bash
cd desktop
```

```
./app_desktop_build
```

WASD 键导航



---

**余烬之痕**

*曾燃烧于灵魂深处的炽烈之火，如今仅余微光闪烁，诉说着未尽的誓言。*

- **类型：** 未完成的武器
- **攻击力：** 0 → ∞ （取决于持有者的决心）
- **耐久度：** 3/99 （在倦怠中逐渐崩坏）
- **重量：** 5.5
- 属性需求：
  - 力量：12
  - 信仰：8
  - 毅力：无上限

**技能：拾遗之志**

消耗所有的信念与耐心，将这未竟之刃重新锻造为独属于自己的武器，属性与形态因人而异。*（注意：过程漫长且孤独，失败即为归于虚无。）*

*“未能成型，但那微弱的光，仍等待着无畏者的锤炼。”*

