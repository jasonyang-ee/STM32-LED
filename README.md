[![Build Binary](https://github.com/jasonyang-ee/STM32-LED/actions/workflows/build-all.yml/badge.svg)](https://github.com/jasonyang-ee/STM32-LED/actions/workflows/build-all.yml)


# 1. LED Class Use Instruction

Project Using STM32L432KC as Example. Test hardware is NUCLEO-L432KC.


## 1.1. Build This Project

- Docker Build
```
docker run -v "{Your_Local_Full_Path}":"/home" jasonyangee/stm32_ubuntu:latest https://github.com/jasonyang-ee/STM32-LED
```
> Visit Here for More Details: https://github.com/jasonyang-ee/STM32-Dockerfile
![Run](doc/img/run.gif)


- Manual Build
```
cmake -B build -G Ninja
cmake --build build
```


## 1.2. PWM

- `Prescaler` x `Overflow` = `Clock Source` = `System Clock` / `Desired PWM Frequency`

- If wanting period be 100 for ease of duty cycle setting:
- 32Mhz / 1KHz / 100 = 320
- => `prescaler`: 320-1, `period`: 100-1, `PWM Frequency`: 1KHz
- If wanting max precision with using max period:
- 32Mhz / 1KHz / (if > 65535) then divide 65535 (16bit period)
- => `prescaler`: 1-1, `period`: 32000-1, `PWM Frequency`: 1KHz
- Use `clock source`: `internal clock (APBx)`

## 1.3. Interrupt Via PWM OC (output compare) signal

- Enable `PWM global interrupt`
- Run `HAL_TIM_PWM_Start_IT(&htimX, TIM_CHANNEL_X)` in `main()`
- Define `void PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)`
- Run `led_user.scheduler()` in the `PWM_PulseFinishedCallback`

## 1.4. LED Class
  - Init object `LED led_user{period, dimmer, Interrupt Frequency}`
  - Passthogh channel CCR with `led_user.setCCR(&htimX.Instance->CCRX);`
  - Ready to use `on()`, `off()`, `toggle()`, `set()`, `breath()`, `blink()`, `rapid()`.

## 1.5. C++ Host
- Optionally, declear `extern object` in `instance.h` to allow object sharing.
- Only define object init in `main.cpp`.


## 1.6. Side Note

Use Regular Expression: `` /\*.*/ `` to remove HAL comments in `main.cpp`


