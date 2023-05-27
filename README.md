# ECE243 Battleship Project
### Brief Overview ###
In this course, I learned about the ARMv7 processor and was tasked with programming a game in **C**. I decided to build one of my favourite childhood games, Battleship, with sleek visuals and compatibility with the DE1-SOC board and PS/2 keyboard.

![image](https://github.com/uzFer/ECE243-Battleship/assets/109243682/0aedfb97-b3a1-4e74-8d74-3d1fddd4563c)

### Key Features ###
- Different screens for each stage of the game including: starting and instruction screens, setup screens, guessing screens (displays hit, miss, or times up depending on guess), and a game over screen for the player who wins.
- Created own font style that can be displayed at any font size on chosen location on the screen.
- Compatible with **PS/2 keyboard** using the spacebar, 'enter' key, up/down/left/right arrows AND **pushbutton keys** and **switches** on DE1-SOC to place guesses or navigate through screens. All inputs used interrupts as opposed to polling.
- Configured interrupts on the **ARM A9 Private Timer** to countdown each turn and display a one-second flicker on the square during setup/guessing screens
