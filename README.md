# **Ping-Pong Game** 

## Componente:
- Arduino Uno R3
- Breadboards x4
- LCD1602 Module
- 8x8 Matrix display
- Potentiometers x2
- Matrix driver
- F-M cables
- Resistors


## *Start*

Inainte de a da start, potentiometrele trebuie calibrate, aduse la valoarea 0.

Pentru a da start, ambii jucatori trebuie sa duca sageata catre cuvantul "START" de pe LCD, folosind potentiometrul.


## *Desfasurarea jocului*

Regulile sunt aceleasi ca la jocul clasic de ping-pong.

### **Extra**

- Mingea se ciocneste de pereti si nu iese afara din tabla.
- Mingea se poate deplasa pe linie dreapta de la un perete spre celalalt, iar ambii jucatori trebuie sa o urmareasca, directia de plecare fiind apoi aleatorie.

### **Punctajul**
Atunci cand unul dintre jucatori nu prinde mingea pe paleta, creste punctajul adversarului.

Dupa cresterea punctajului, mingea este plasata la jumatatea tablei de joc si porneste intr-o directie aleatorie.

Jocul se incheie cand unul dintre jucatori acumuleaza 6 puncte.

Punctajul este in permanenta afisat pe LCD.

### **Castigator**
Dupa ce unul dintre jucatori a acumulat 6 puncte, jocul se incheie si pe LCD este anuntat castigatorul in formatul: winner: 1/2.

### *Replay*
Pentru a ajunge inapoi la meniul de start, ambele potentiometre trebuie sa fie setate pe valoarea 0.

Gasiti [aici](https://drive.google.com/open?id=1NfRior8KgMNvDu4v_wtmC3IF7S7RgskA) un demo al jocului.