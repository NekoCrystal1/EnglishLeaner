# English Learner 产品优化说明

## 项目定位

本项目当前是 C/S + B/S 混合架构：Spring Boot 统一提供 API，Vue Web 作为轻量学习入口，Qt 客户端预留离线学习能力。短期最适合先把“背单词”做成完整闭环，再把测验、排行榜、内容管理、AI 对话、听说训练等模块作为可插拔能力逐步接入。

## 参考产品范式

- Anki：采用新卡每日上限、到期复习优先、学习/复习间隔和熟悉度按钮的机制，适合做长期记忆调度。参考：https://docs.ankiweb.net/deck-options.html
- SuperMemo SM-2：用 0-5 熟悉度评分、ease factor 和复习间隔增长来决定下次复习时间，适合当前数据模型快速落地。参考：https://www.super-memory.com/english/ol/sm2.htm
- Quizlet Learn：强调目标导向的个性化学习路径，把“选择计划 -> 学习 -> 即时反馈”做成连续 session。参考：https://help.quizlet.com/hc/en-us/articles/360030986971
- Duolingo/同类产品：打卡、积分、排行榜用于维持动机，但不应压过学习主任务。参考：https://blog.duolingo.com/how-streaks-keep-duolingo-learners-committed-to-their-language-goals/

## 当前交付边界

本轮只把背单词模块做成可用的学习闭环：

- 词书选择和每日新词/复习配额。
- 到期复习优先，然后补充未见过的新词。
- 卡片式学习界面，先回忆再显示释义。
- 0-5 熟悉度评分映射为“忘记/模糊/认识/太简单”。
- 根据评分更新掌握度、ease factor、下次复习时间、积分和今日统计。
- 保持 `correct` 字段兼容旧客户端，同时把 `quality` 写入学习事件扩展字段。

## 后续模块预留

- 测验模块：复用单词状态，支持拼写、选择、听音辨义、例句填空。
- 内容模块：扩展词书导入、词根词缀、例句、音频、图片。
- 数据分析：按词书、难度、正确率、遗忘词、连续学习天数生成学习报告。
- 社区/排行榜：只展示激励数据，避免影响记忆调度的核心算法。
- Qt 离线端：继续使用 `clientEventId` 和 `syncVersion` 做幂等同步。
- AI 模块：后续可基于当前词书和弱项词生成例句、短文、口语问答。
