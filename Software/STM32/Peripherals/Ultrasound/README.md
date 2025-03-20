For the ultrasound sensor:
- TIM2 is set as PWM generator (10 µs of high state), this is wired to the "trig" pin (on D13). It is meant to trigger a measurement of the HCSR04
- TIM4 is set as Input Capture Timer. It has an interrupt that triggers at each edge on D10. With a rising edge and a falling edge, we compute the time elapsed. This gives us a value proportionnal to the distance.
- TIM5 is set a a time reference (counts each µs). It is useful to compute a pulse's length.