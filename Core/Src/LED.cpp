#include "LED.hpp"

/**
 * @brief Construct a new LED::LED object
 *
 * Light max brightness level set at 100, scale set at. Scale is used to divide
 * brightness.
 *
 * @warning Initialize LED::LED object before main(). Then port must be set
 * after HAL_TIM_PWM_Start() using setPort().
 * @see setPort() for setting HAL PWM register
 * @see setScale() to set scale/divider of brightness
 * @see setLevel() to set brightness base value
 */
LED::LED(uint16_t level, uint16_t scale) {
    m_level = level;
    m_scale = scale;
    m_status = 0b00000000;
    m_breath_itr = 0;
}

/**
 * @brief Destroy the LED::LED object
 *
 */
LED::~LED() { *m_CCR = 0; }

/**
 * @brief Initialize with passing Timer PWM CCR register reference.
 *
 * Must use after HAL_TIM_PWM_Start().
 * Example: Timer_1 Channel_3 will be &htim1.Instance->CCR3.
 * @param port Timer PWM CCR register.
 */
void LED::setCCR(__IO uint32_t *CCR) { m_CCR = CCR; }

// Bitwise operation setting status
void LED::on() { m_status = 1u; }
void LED::off() { m_status = 0u; }
void LED::toggle() { m_status ^= 1u; }
void LED::set(bool state) { m_status |= (state ? 1u : 0u); }

/**
 * @brief Set brightness scale.
 *
 * @param value
 */
void LED::setScale(uint16_t value) { m_scale = value; }

/**
 * @brief Set brightness max level.
 *
 * @param value
 */
void LED::setLevel(uint16_t value) { m_level = value; }

/**
 * @brief Set LED max brightness to half.
 *
 */
void LED::half() { m_level = 50; }

/**
 * @brief Use in 20Hz timer interrupt to periodically update LED brightness to
 * emulate breathing and blinking.
 *
 * Run this in HAL_TIM_PeriodElapsedCallback() for the 20Hz timer.
 */
void LED::scheduler() {
    if (m_schedule == 0) {
        switch (m_status) {
            case 0:  // Solid ON
                *m_CCR = 0;
                break;
            case 1:  // Solid OFF
                *m_CCR = m_level / m_scale;
                break;
            case 2:  // Breathing LED Logic
                if (++m_breath_itr < 25)
                    m_level = m_breath[m_breath_itr];
                else
                    m_breath_itr = 0;
                *m_CCR = m_level / m_scale;
                break;
            case 4:  // Slow Blinking LED Logic
                if (m_blink_timer > 5) {
                    toggle();
                    m_blink_timer = 0;
                } else
                    m_blink_timer++;
                break;
            case 8:  // Fast Blinking LED Logic
                if (m_rapid_timer > 1) {
                    toggle();
                    m_rapid_timer = 0;
                } else
                    m_rapid_timer++;
                break;
        }
    }
    if (m_schedule++ > 50) m_schedule = 0;
}

/*
Toggle 8 Bits Status Code
        |===  3  =======  2  =======  1  ======== 0 ====|
Bit		|	Rapid	|	Blink	|	Breath | Solid	|
        |===============================================|
*/

/**
 * @brief Start breathing effect.
 *
 * LED breathing turn on. The brightness will change based on scheduler().
 */
void LED::breath() { m_status = (1u << 2); }

/**
 * @brief Start slow blinking.
 *
 * LED blinking turn on. The on/off will change based on scheduler().
 */
void LED::blink() {
    m_status = (1u << 3);
    setLevel(100);
}

/**
 * @brief Start fast blinking.
 *
 * Led blinking turn on. The on/off will change based on scheduler().
 */
void LED::rapid() {
    m_status = (1u << 4);
    setLevel(100);
}
