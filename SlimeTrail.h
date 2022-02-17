#ifndef SLIMETRAIL_H
#define SLIMETRAIL_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class SlimeTrail;
}
QT_END_NAMESPACE

class Hole;

class SlimeTrail : public QMainWindow {
    Q_OBJECT

public:
    enum Player {
        RedPlayer,
        BluePlayer
    };
    Q_ENUM(Player)

    SlimeTrail(QWidget *parent = nullptr);
    virtual ~SlimeTrail();

signals:
    void turnEnded();

private:
    Ui::SlimeTrail *ui;
    Player m_player;
    Hole* m_board[8][8];
    int avaiable_positions_on_neighborhood[8][2]; // duplas de posições

private slots:
    void play(int id);
    bool confirmNeighbor(int id);
    void updateNeighborhood();
    void switchPlayer();
    void reset();

    void showResult(int i);
    void showAbout();
    void updateStatusBar();
};

#endif // SLIMETRAIL_H
