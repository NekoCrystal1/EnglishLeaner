package com.englishlearner.server;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class EnglishLearnerServerApplication {

    /** 启动 Spring Boot 后端，并扫描当前包下的组件。 */
    public static void main(String[] args) {
        SpringApplication.run(EnglishLearnerServerApplication.class, args);
    }
}
