<template>
  <div class="auth-page">
    <section class="hero-card">
      <h1>交互式英语自学系统</h1>
      <p>支持账号登录、在线刷题、实时积分与排行榜。</p>
      <div class="hero-tags">
        <el-tag type="primary">JWT 鉴权</el-tag>
        <el-tag type="success">实时积分</el-tag>
        <el-tag type="warning">Web 轻量入口</el-tag>
      </div>
    </section>

    <section class="form-card">
      <el-tabs v-model="activeTab" stretch>
        <el-tab-pane label="登录" name="login">
          <el-form ref="loginFormRef" :model="loginForm" :rules="loginRules" label-position="top">
            <el-form-item prop="username" label="用户名">
              <el-input
                v-model="loginForm.username"
                placeholder="请输入用户名"
                clearable
                @keyup.enter="submitLogin"
              />
            </el-form-item>
            <el-form-item prop="password" label="密码">
              <el-input
                v-model="loginForm.password"
                placeholder="请输入密码"
                type="password"
                show-password
                @keyup.enter="submitLogin"
              />
            </el-form-item>
            <el-button :loading="authStore.loading" type="primary" class="action-btn" @click="submitLogin">
              登录
            </el-button>
          </el-form>
        </el-tab-pane>

        <el-tab-pane label="注册" name="register">
          <el-form ref="registerFormRef" :model="registerForm" :rules="registerRules" label-position="top">
            <el-form-item prop="username" label="用户名">
              <el-input v-model="registerForm.username" placeholder="2-32 个字符" clearable />
            </el-form-item>
            <el-form-item prop="password" label="密码">
              <el-input v-model="registerForm.password" type="password" show-password placeholder="6-64 个字符" />
            </el-form-item>
            <el-form-item prop="confirmPassword" label="确认密码">
              <el-input
                v-model="registerForm.confirmPassword"
                type="password"
                show-password
                @keyup.enter="submitRegister"
              />
            </el-form-item>
            <el-button :loading="authStore.loading" type="success" class="action-btn" @click="submitRegister">
              注册并登录
            </el-button>
          </el-form>
        </el-tab-pane>
      </el-tabs>
    </section>
  </div>
</template>

<script setup>
import { computed, reactive, ref } from "vue";
import { ElMessage } from "element-plus";
import { useRoute, useRouter } from "vue-router";
import { useAuthStore } from "../stores/auth";

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();

const activeTab = ref("login");
const loginFormRef = ref();
const registerFormRef = ref();

const loginForm = reactive({
  username: "",
  password: ""
});

const registerForm = reactive({
  username: "",
  password: "",
  confirmPassword: ""
});

const redirectPath = computed(() => {
  const fromQuery = route.query.redirect;
  if (!fromQuery || typeof fromQuery !== "string" || fromQuery === "/auth") {
    return "/";
  }
  return fromQuery;
});

const loginRules = {
  username: [
    { required: true, message: "请输入用户名", trigger: "blur" }
  ],
  password: [
    { required: true, message: "请输入密码", trigger: "blur" }
  ]
};

const registerRules = {
  username: [
    { required: true, message: "请输入用户名", trigger: "blur" },
    { min: 2, max: 32, message: "用户名长度需要在 2-32 之间", trigger: "blur" }
  ],
  password: [
    { required: true, message: "请输入密码", trigger: "blur" },
    { min: 6, max: 64, message: "密码长度需要在 6-64 之间", trigger: "blur" }
  ],
  confirmPassword: [
    {
      validator: (_, value, callback) => {
        if (!value) {
          callback(new Error("请再次输入密码"));
          return;
        }
        if (value !== registerForm.password) {
          callback(new Error("两次输入的密码不一致"));
          return;
        }
        callback();
      },
      trigger: "blur"
    }
  ]
};

async function submitLogin() {
  if (!loginFormRef.value) {
    return;
  }

  try {
    await loginFormRef.value.validate();
    await authStore.login({
      username: loginForm.username,
      password: loginForm.password
    });
    ElMessage.success("登录成功");
    router.replace(redirectPath.value);
  } catch (error) {
    if (error?.message) {
      ElMessage.error(error.message);
    }
  }
}

async function submitRegister() {
  if (!registerFormRef.value) {
    return;
  }

  try {
    await registerFormRef.value.validate();
    await authStore.registerAndLogin({
      username: registerForm.username,
      password: registerForm.password
    });
    ElMessage.success("注册并登录成功");
    router.replace("/");
  } catch (error) {
    if (error?.message) {
      ElMessage.error(error.message);
    }
  }
}
</script>

<style scoped>
.auth-page {
  min-height: 100vh;
  display: grid;
  grid-template-columns: 1.1fr 1fr;
  align-items: center;
  gap: 24px;
  max-width: 1100px;
  margin: 0 auto;
  padding: 24px;
}

.hero-card {
  padding: 32px;
  border-radius: 24px;
  background:
    radial-gradient(circle at 20% 20%, rgba(59, 130, 246, 0.22), transparent 60%),
    linear-gradient(130deg, #ffffff, #f0f9ff);
  border: 1px solid #dbeafe;
}

.hero-card h1 {
  margin: 0 0 12px;
  font-size: 36px;
  line-height: 1.2;
}

.hero-card p {
  margin: 0;
  color: #4b5563;
}

.hero-tags {
  margin-top: 20px;
  display: flex;
  gap: 8px;
  flex-wrap: wrap;
}

.form-card {
  padding: 20px;
  border-radius: 20px;
  background: rgba(255, 255, 255, 0.94);
  border: 1px solid #e5e7eb;
}

.action-btn {
  width: 100%;
}

@media (max-width: 960px) {
  .auth-page {
    grid-template-columns: 1fr;
    padding: 12px;
  }

  .hero-card h1 {
    font-size: 28px;
  }
}
</style>
