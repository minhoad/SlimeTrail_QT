#include "SlimeTrail.h"
#include "ui_SlimeTrail.h"

#include <QDebug>
#include <QMessageBox>
#include <QActionGroup>
#include <QSignalMapper>

SlimeTrail::Player otherPlayer(SlimeTrail::Player player) {
    return (player == SlimeTrail::RedPlayer ?
                    SlimeTrail::BluePlayer : SlimeTrail::RedPlayer);
}

SlimeTrail::SlimeTrail(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::SlimeTrail),
      m_player(SlimeTrail::RedPlayer) {

    ui->setupUi(this);

    QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));
    QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

    QSignalMapper* map = new QSignalMapper(this);
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QString holeName = QString("hole%1%2").arg(row).arg(col);
            Hole* hole = this->findChild<Hole*>(holeName);
            Q_ASSERT(hole != nullptr);
            Q_ASSERT(hole->row() == row && hole->col() == col);

            m_board[row][col] = hole;

            int id = row * 8 + col;
            map->setMapping(hole, id);
            QObject::connect(hole, SIGNAL(clicked(bool)), map, SLOT(map()));
        }
    }
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));
#else
    QObject::connect(map, SIGNAL(mappedInt(int)), this, SLOT(play(int)));
#endif

    // When the turn ends, switch the player.
    QObject::connect(this, SIGNAL(turnEnded()), this, SLOT(switchPlayer()));

    this->reset();

    this->adjustSize();
    this->setFixedSize(this->size());
}

SlimeTrail::~SlimeTrail() {
    delete ui;
}

void SlimeTrail::play(int id) {
    Hole* hole = m_board[id / 8][id % 8];
    qDebug() << "clicked on: " << hole->objectName();
    if(confirmNeighbor(id)){
        for(int i = 0 ; i < 8 ; i++){
            for(int j = 0 ; j < 8 ; j++){ // preencho a antiga posição de preto
                if(m_board[i][j]->state() == Hole::WhiteState){
                    m_board[i][j]->setState(Hole::BlackState);
                    break;
                }
            }
        }
        hole->setState(Hole::WhiteState); // coloco a nova posição
        emit turnEnded();
    }
    int counter_avaiable_positions = 0;
    for(int i = 0 ; i < 8 ; i++){
        if(avaiable_positions_on_neighborhood[i][0] == -1 && avaiable_positions_on_neighborhood[i][1] == -1){
            counter_avaiable_positions++;
        }
    }
    if(counter_avaiable_positions==8)showResult(0);
    if(m_board[7][0]->state() == Hole::WhiteState || m_board[0][7]->state() == Hole::WhiteState){
        showResult(1);
    }
    // move
    // setar a antiga posição como preta
    // se não não posso acabar o turno, pois o jogador tem que se mover
    // lembrar da regra de empate
    // conferir condições de vitória
}
bool SlimeTrail::confirmNeighbor(int id){
    updateNeighborhood(); // verifica a vizinhança do ponto branco
    for(int i = 0 ; i < 8 ; i++){
        if((avaiable_positions_on_neighborhood[i][0] == (id / 8)) && (avaiable_positions_on_neighborhood[i][1] == (id % 8))){
            return true;
        }
    }
    return false;
}

void SlimeTrail::updateNeighborhood(){ // define as posições que pode se mover
    int position_white[2];
    bool flag = false;
    for(int i = 0 ; i < 8 ; i++){ // capturando a posição em que a peça branca se encontra
        for(int j = 0 ; j < 8 ; j++){
            if(m_board[i][j]->state() == Hole::WhiteState){
                position_white[0] = i;
                position_white[1] = j;
                if( ((i-1) < 8 && (i-1) >= 0) && ((j-1) < 8 && (j-1) >= 0) ){
                    avaiable_positions_on_neighborhood[0][0] = (m_board[i-1][j-1]->state() == Hole::EmptyState)? i-1 : -1;
                    avaiable_positions_on_neighborhood[0][1] = (m_board[i-1][j-1]->state() == Hole::EmptyState)? j-1 : -1;
                }else{ // caso esteja fora do tabuleiro
                    avaiable_positions_on_neighborhood[0][0] = -1;
                    avaiable_positions_on_neighborhood[0][1] = -1;
                }
                if( ((i-1) < 8 && (i-1) >= 0) && ((j) < 8 && (j) >= 0) ){
                    avaiable_positions_on_neighborhood[1][0] = (m_board[i-1][j]->state() == Hole::EmptyState)? i-1 : -1;
                    avaiable_positions_on_neighborhood[1][1] = (m_board[i-1][j]->state() == Hole::EmptyState)? j : -1;
                }else{ // caso esteja fora do tabuleiro
                    avaiable_positions_on_neighborhood[1][0] = -1;
                    avaiable_positions_on_neighborhood[1][1] = -1;
                }
                if( ((i-1) < 8 && (i-1) >= 0) && ((j+1) < 8 && (j+1) >= 0) ){
                    avaiable_positions_on_neighborhood[2][0] = (m_board[i-1][j+1]->state() == Hole::EmptyState)? i-1 : -1;
                    avaiable_positions_on_neighborhood[2][1] = (m_board[i-1][j+1]->state() == Hole::EmptyState)? j+1 : -1;
                }else{ // caso esteja fora do tabuleiro
                    avaiable_positions_on_neighborhood[2][0] = -1;
                    avaiable_positions_on_neighborhood[2][1] = -1;
                }
                if( ((i) < 8 && (i) >= 0) && ((j-1) < 8 && (j-1) >= 0) ){
                    avaiable_positions_on_neighborhood[3][0] = (m_board[i][j-1]->state() == Hole::EmptyState)? i : -1;
                    avaiable_positions_on_neighborhood[3][1] = (m_board[i][j-1]->state() == Hole::EmptyState)? j-1 : -1;
                }else{ // caso esteja fora do tabuleiro
                    avaiable_positions_on_neighborhood[3][0] = -1;
                    avaiable_positions_on_neighborhood[3][1] = -1;
                }
                if( ((i) < 8 && (i) >= 0) && ((j+1) < 8 && (j+1) >= 0) ){
                    avaiable_positions_on_neighborhood[4][0] = (m_board[i][j+1]->state() == Hole::EmptyState)? i : -1;
                    avaiable_positions_on_neighborhood[4][1] = (m_board[i][j+1]->state() == Hole::EmptyState)? j+1 : -1;
                }else{ // caso esteja fora do tabuleiro
                    avaiable_positions_on_neighborhood[4][0] = -1;
                    avaiable_positions_on_neighborhood[4][1] = -1;
                }
                if( ((i+1) < 8 && (i+1) >= 0) && ((j-1) < 8 && (j-1) >= 0) ){
                    avaiable_positions_on_neighborhood[5][0] = (m_board[i+1][j-1]->state() == Hole::EmptyState)? i+1 : -1;
                    avaiable_positions_on_neighborhood[5][1] = (m_board[i+1][j-1]->state() == Hole::EmptyState)? j-1 : -1;
                }else{ // caso esteja fora do tabuleiro
                    avaiable_positions_on_neighborhood[5][0] = -1;
                    avaiable_positions_on_neighborhood[5][1] = -1;
                }
                if( ((i+1) < 8 && (i+1) >= 0) && ((j) < 8 && (j) >= 0)){
                    avaiable_positions_on_neighborhood[6][0] = (m_board[i+1][j]->state() == Hole::EmptyState)? i+1 : -1;
                    avaiable_positions_on_neighborhood[6][1] = (m_board[i+1][j]->state() == Hole::EmptyState)? j : -1;
                }else{ // caso esteja fora do tabuleiro
                    avaiable_positions_on_neighborhood[6][0] = -1;
                    avaiable_positions_on_neighborhood[6][1] = -1;
                }
                if( ((i+1) < 8 && (i+1) >= 0) && ((j+1) < 8 && (j+1) >= 0) ){
                    avaiable_positions_on_neighborhood[7][0] = (m_board[i+1][j+1]->state() == Hole::EmptyState)? i+1 : -1;
                    avaiable_positions_on_neighborhood[7][1] = (m_board[i+1][j+1]->state() == Hole::EmptyState)? j+1 : -1;
                }else{ // caso esteja fora do tabuleiro
                    avaiable_positions_on_neighborhood[7][0] = -1;
                    avaiable_positions_on_neighborhood[7][1] = -1;
                }
                flag = true;
                break;
            }
        }
        if(flag)break;
    }
}

void SlimeTrail::switchPlayer() {
    // Switch the player.
    m_player = otherPlayer(m_player);

    // Finally, update the status bar.
    this->updateStatusBar();
}

void SlimeTrail::reset() {
    // Reset board.
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Hole* hole = m_board[row][col];
            hole->reset();

            // FIXME: Only neighboor holes should be marked.
            hole->setMarked(true);
        }
    }

    // Mark the starting position.
    m_board[3][4]->setState(Hole::WhiteState);

    // Reset the player.
    m_player = SlimeTrail::RedPlayer;

    // Finally, update the status bar.
    this->updateStatusBar();
}

void SlimeTrail::showAbout() {
    QMessageBox::information(this, tr("Sobre"), tr("Rastros\n\nDarmes Araujo Dias - darmes2000@hotmail.com"));
}

void SlimeTrail::showResult(int i){
   if(i==1)
   (m_player==SlimeTrail::RedPlayer)?
   QMessageBox::information(this,tr("Vencedor"), tr("Parabéns, o Jogador Vermelho venceu.")):
   QMessageBox::information(this,tr("Vencedor"), tr("Parabéns, o Jogador Azul venceu."));
   else QMessageBox::information(this,tr("Empate"),tr("O jogo empatou!"));
   reset();
}

void SlimeTrail::updateStatusBar() {
    QString player(m_player == SlimeTrail::RedPlayer ? "Vermelho" : "Azul");
    ui->statusbar->showMessage(tr("Vez do Jogador %2").arg(player));
}
