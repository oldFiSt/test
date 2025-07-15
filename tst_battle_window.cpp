// #include <QtTest>
// #include "battlewindow.h"

// class TestBattleWindow : public QObject
// {
//     Q_OBJECT

// private slots:
//     //void testValidVerticalPlacement();
//     // void testInvalidDiagonalPlacement();
//     // void testInvalidTouchingOtherShip();


//     void cleanupTestCase() {
//         qDebug() << "cleanupTestCase called";
//     }

//     void testMarkPlayerDestroyedShip();
//     //void testMarkSurroundingArea();
// };

// //проверим, что клетки вокруг корабля становятся белыми и неактивными
// // void TestBattleWindow::testMarkSurroundingArea()
// // {
// //     qDebug() << "Running testMarkSurroundingArea";
// //     BattleWindow win({}, nullptr);

// //     // Создаем корабль в центре
// //     QList<QPoint> ship = { QPoint(5, 5) };

// //     // Инициализируем поле кнопками с hasShip = true на корабле
// //     for (int r = 0; r < 10; ++r)
// //         for (int c = 0; c < 10; ++c) {
// //             win.enemyField[r][c] = new QPushButton();
// //             win.enemyField[r][c]->setProperty("hasShip", false);
// //             win.enemyField[r][c]->setEnabled(true);
// //         }

// //     for (const QPoint& pt : ship)
// //         win.enemyField[pt.x()][pt.y()]->setProperty("hasShip", true);

// //     // Вызов метода
// //     win.markSurroundingArea(ship);

// //     // Проверяем, что вокруг точки (5,5) клетки стали белыми и неактивными
// //     for (int dr = -1; dr <= 1; ++dr) {
// //         for (int dc = -1; dc <= 1; ++dc) {
// //             int nr = 5 + dr;
// //             int nc = 5 + dc;
// //             if (nr >= 0 && nr < 10 && nc >= 0 && nc < 10) {
// //                 QPushButton* btn = win.enemyField[nr][nc];
// //                 if (dr == 0 && dc == 0) {
// //                     QVERIFY(btn->property("hasShip").toBool());
// //                 } else {
// //                     QCOMPARE(btn->isEnabled(), false);
// //                     QVERIFY(btn->styleSheet().contains("white"));
// //                 }
// //             }
// //         }
// //     }
// // }


// // void TestBattleWindow::testCheckPlayerShipDestroyed()
// // {
// //     BattleWindow win({}, nullptr);

// //     QList<QPoint> ship = { QPoint(1, 1), QPoint(1, 2) };
// //     win.playerShipsGrouped.append(ship);
// //     win.playerShipsGroupedFOREVER.append(ship);

// //     // Помечаем корабль на поле
// //     for (const QPoint& pt : ship) {
// //         win.playerField[pt.x()][pt.y()] = new QPushButton();
// //         win.playerField[pt.x()][pt.y()]->setProperty("hasShip", true);
// //         win.playerField[pt.x()][pt.y()]->setEnabled(true);
// //     }

// //     int initialShipsAlive = win.playerShipsAlive = 1;

// //     win.checkPlayerShipDestroyed(QPoint(1, 1));
// //     QCOMPARE(win.playerShipsAlive, initialShipsAlive); // Пока не убрали весь корабль

// //     win.checkPlayerShipDestroyed(QPoint(1, 2));
// //     QCOMPARE(win.playerShipsAlive, initialShipsAlive - 1); // Корабль уничтожен
// // }


// //Тесты на методы, связанные с игроком
// void TestBattleWindow::testMarkPlayerDestroyedShip()
// {
//     BattleWindow win({}, nullptr);

//     QList<QPoint> ship = { QPoint(3, 3) };

//     for (int r = 0; r < 10; ++r)
//         for (int c = 0; c < 10; ++c) {
//             win.playerField[r][c] = new QPushButton();
//             win.playerField[r][c]->setProperty("hasShip", false);
//             win.playerField[r][c]->setEnabled(true);
//         }

//     for (const QPoint& pt : ship)
//         win.playerField[pt.x()][pt.y()]->setProperty("hasShip", true);

//     win.markPlayerDestroyedShip(ship);

//     for (int dr = -1; dr <= 1; ++dr)
//         for (int dc = -1; dc <= 1; ++dc) {
//             int nr = 3 + dr;
//             int nc = 3 + dc;
//             if (nr >= 0 && nr < 10 && nc >= 0 && nc < 10) {
//                 QPushButton* btn = win.playerField[nr][nc];
//                 if (!(dr == 0 && dc == 0)) {
//                     QCOMPARE(btn->isEnabled(), false);
//                     QVERIFY(btn->styleSheet().contains("white"));
//                 }
//             }
//         }
// }



// // void TestBattleWindow::testValidVerticalPlacement()
// // {
// //     BattleWindow win({}, nullptr);
// //     QList<QPoint> ship = { QPoint(1, 1), QPoint(2, 1), QPoint(3, 1) };
// //     QVERIFY(win.isValidEnemyShipPlacement(ship));
// // }


// // void TestBattleWindow::testInvalidDiagonalPlacement()
// // {
// //     BattleWindow win({}, nullptr);
// //     QList<QPoint> ship = { QPoint(1, 1), QPoint(2, 2), QPoint(3, 3) };
// //     QVERIFY(!win.isValidEnemyShipPlacement(ship));
// // }


// // void TestBattleWindow::testInvalidTouchingOtherShip()
// // {
// //     BattleWindow win({}, nullptr);

// //     QList<QPoint> existingShip = { QPoint(4, 4), QPoint(5, 4) };
// //     win.enemyShipsGrouped.append(existingShip);
// //     win.enemyShipsGroupedFOREVER.append(existingShip);

// //     for (const QPoint& pt : existingShip) {
// //         win.enemyField[pt.x()][pt.y()] = new QPushButton();
// //         win.enemyField[pt.x()][pt.y()]->setProperty("hasShip", true);
// //     }

// //     QList<QPoint> newShip = { QPoint(6, 4), QPoint(7, 4) };
// //     QVERIFY(!win.isValidEnemyShipPlacement(newShip));
// // }

// // Создаёт точку входа для теста, без собственного main()
// QTEST_MAIN(TestBattleWindow)


// #include "tst_battle_window.moc"
