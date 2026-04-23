import { createApp } from "vue";
import { createPinia } from "pinia";
import ElementPlus from "element-plus";
import "element-plus/dist/index.css";
import { ArrowDown } from "@element-plus/icons-vue";
import App from "./App.vue";
import { router, setupRouterGuard } from "./router";
import "./styles.css";

const app = createApp(App);
const pinia = createPinia();

app.component("ArrowDown", ArrowDown);

app.use(pinia);
setupRouterGuard(router, pinia);
app.use(router);
app.use(ElementPlus);

app.mount("#app");
