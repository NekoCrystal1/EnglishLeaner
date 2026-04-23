# EnglishLearner Web

基于 Vue3 + Vite 的 Web 端实现，已对接当前后端接口：

- `POST /api/v1/auth/register`
- `POST /api/v1/auth/login`
- `GET /api/v1/users/me`
- `GET /api/v1/quiz/next`
- `POST /api/v1/quiz/submit`
- `GET /api/v1/ranking/top`

## 功能

- 登录与注册（JWT 登录态持久化）
- 学习概览（用户信息、我的排名、Top 3 预览）
- 刷题练习（随机题目、提交答案、即时反馈积分）
- 排行榜（表格 + ECharts 柱状图）

## 启动

1. 安装依赖

```bash
npm install
```

2. （可选）配置后端地址

```bash
cp .env.example .env
```

`VITE_API_BASE_URL` 默认可以指向 `http://localhost:8080/api/v1`。  
若不配置，开发模式会使用 `vite.config.js` 中的 `/api` 反向代理到 `http://localhost:8080`。

3. 启动开发服务

```bash
npm run dev
```

4. 打包

```bash
npm run build
```
