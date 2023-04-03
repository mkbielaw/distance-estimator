figure(1)
plot(mx.time, mx.Data, 'color',[0.8890 0.7940 0.1250])
hold on
plot(mx.time, ex, 'color',[0.8500 0.3250 0.0980],'LineWidth',1)
plot(rx.time, rx.Data,'color',[0 0.4470 0.7410],'LineWidth',1.5,'LineStyle','--')
legend('Zmierzone przyspieszenie','Estymowane przyspieszenie','Rzeczywiste przyspieszenie')
title('Przyspieszenie w osi x','FontName', 'Times New Roman')
xlabel('t [s]','FontName', 'Times New Roman')
ylabel('a [m/s^2]','FontName', 'Times New Roman')
hold off

figure(2)
plot(my.time, my.Data, 'color',[0.8890 0.7940 0.1250])
hold on
plot(my.time, ey, 'color',[0.8500 0.3250 0.0980],'LineWidth',1)
plot(ry.time, ry.Data,'color',[0 0.4470 0.7410],'LineWidth',1.5,'LineStyle','--')
legend('Zmierzone przyspieszenie','Estymowane przyspieszenie','Rzeczywiste przyspieszenie')
title('Przyspieszenie w osi y','FontName', 'Times New Roman')
xlabel('t [s]','FontName', 'Times New Roman')
ylabel('a [m/s^2]','FontName', 'Times New Roman')
hold off

figure(3)
plot(mv.time, mv.Data, 'color',[0.8290 0.6940 0.1250],'LineWidth',1)
hold on
plot(mv.time, ev, 'color',[0.8500 0.3250 0.0980],'LineWidth',1)
plot(rv.time, rv.Data,'color',[0 0.4470 0.7410],'LineWidth',1.5,'LineStyle','--')
legend('Zmierzona pr?dko??','Estymowana pr?dko??','Rzeczywista pr?dko??')
title('Pr?dko?? ca?kowita','FontName', 'Times New Roman')
xlabel('t [s]','FontName', 'Times New Roman')
ylabel('v [m/s]','FontName', 'Times New Roman')
hold off

figure(4)
plot(ms.time, ms.Data, 'color',[0.8290 0.6940 0.1250],'LineWidth',1)
hold on
plot(ms.time, es, 'color',[0.8500 0.3250 0.0980],'LineWidth',1)
plot(rs.time, rs.Data,'color',[0 0.4470 0.7410],'LineWidth',1.5,'LineStyle','--')
legend('Zmierzone przemieszczenie','Estymowane przemieszczenie','Rzeczywiste przemieszczenie')
title('Przemieszczenie ca?kowite','FontName', 'Times New Roman')
xlabel('t [s]','FontName', 'Times New Roman')
ylabel('s [m]','FontName', 'Times New Roman')
hold off

figure(5)
plot(xs,ys)