#ifndef GAME_H
#define GAME_H
#include <QString>
#include <QFile>
#include <QObject>
#include "timer.h"

/*
*******************************************************************************
Class Game: public QObject
Contém a lógica do jogo "quebra-cabeça de deslizar". A ser instanciado pelo
arquivo main.qml
-------------------------------------------------------------------------------
Signals:
    (usados para acionar as funções de atualização de telas na interface)
    void changedHOFContents();
    void changedUpdateBoard();

public Q_SLOTS:
    (acionados pelas ações de cliques em botões da interface)
    void startGame();
    void buttonClickedOnDisplay(int buttonIndex);
    void getPlayerName(QString newName);

Q_PROPERTIES: (registradas no arquivo QML para os textos dos botões e do Hall da Fama)
    Q_PROPERTY(QString hOFChange READ getHOFChange WRITE formHallOfFameText NOTIFY changedHOFContents)
    Q_PROPERTY(QString updateBoard NOTIFY changedUpdateBoard)

Métodos que podem ser chamados pelo QML:
    Q_INVOKABLE QString getTileTexts(int tileNumber); //verifica o texto de uma peça do quebra-cabeça
    Q_INVOKABLE QString getStartGameBtnText(); //atualiza botão para mostrar ou não os pontos do jogador

*******************************************************************************
*/

class Game: public QObject
{
    Q_OBJECT
    //adicionar propriedades para controlar a escrita nos botões e no Hall da Fama
    Q_PROPERTY(QString hOFChange READ getHOFChange WRITE formHallOfFameText NOTIFY changedHOFContents)
    Q_PROPERTY(QString updateBoard NOTIFY changedUpdateBoard)

public:
    explicit Game(QObject *parent = nullptr);
    ~Game();
    static const int NUMBER_OF_ROWS = 4; //número de linhas do quebra-cabeça.
    static const int NUMBER_OF_COLUMNS = 4; //número de colunas do quebra-cabeça.
    static const int SENTINEL = 16; //Sentinela guarda o código do espaço vazio para o quebra-cabeça.
    static const int HALL_OF_FAME_MAX = 7; //Máximo número de jogadores que podem ser exibidos no Hall da Fama.
    void shuffleGame(); //embaralha as peças de 1 a 15, mantendo a sentinela intacta.
    bool hasSolution(); //define se o arranjo sorteado é possível de resolver.
    int getEmptySpace(int indexLinha, int indexColuna); //retorna a posição do espaço vazio, se existir
    void swapTwo(int indexRowA, int indexColumnA, int indexRowB, int indexColumnB); //troca duas peças quaisquer
    bool isComplete(); //quebra cabeça já foi resolvido?
    void updateTexts(); //transfere os valores das peças para o array que será exibido na interface.
    void loadHallOfFame(); //faz a leitura do arquivo hall_of_fame.pzl
    void saveHallOfFame(); //salva no arquivo hall_of_fame.pzl
    void sortHallOfFame(); //ordena o Hall da Fama quanto a movimentos e tempo
    Q_INVOKABLE QString getTileTexts(int tileNumber); //verifica o texto de uma peça do quebra-cabeça
    Q_INVOKABLE QString getStartGameBtnText(); //atualiza botão para mostrar ou não os pontos do jogador
    void endGame(); //termina o jogo. SSalva os dados do jogador no Hall da fama.
    void buttonClicked(int indexRow, int indexColumn);

    QString getHOFChange(); //qml getter para Hall da Fama

public Q_SLOTS:
    void startGame(); //reinicializa parâmetros de usuário (movimentos, tempo). Começa novo jogo
    void buttonClickedOnDisplay(int buttonIndex); //um botão foi clicado na interface
    void getPlayerName(QString newName); //salvar nome de usuário.

private:
    int positions[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS]; //As peças do quebra-cabeça, de 1 a 15, e sentinela.
    QString texts[NUMBER_OF_ROWS*NUMBER_OF_COLUMNS]; //textos que devem ser impressos na interface
    int convertToArray(int indexRow, int indexColumn); //converte de índice bidimensional para unidimensional.
    QString playerName;
    int playerMoves;
    int playerTime;
    Timer playerClock;
    QString hallOfFameText; //concatenar todas as entradas do Hall da Fama
    QString playGameBtnText; //contém PLAY GAME ou a pontuação do jogador

    void formHallOfFameText(QString text); //atualizar hallOfFameText

    struct playerData
    {
        //Para dados de usuário a serem manipulados no arquivo hall_of_fame.pzl
        QString name;
        int time;
        int moves;
    };
    playerData fileData [HALL_OF_FAME_MAX+1]; //lista dos recordes
    playerData sortedByTime [HALL_OF_FAME_MAX+1]; //fileData sorteada por tempo
    playerData sortedByMoves [HALL_OF_FAME_MAX+1]; //fileData sorteada por movimentos
    //funções de comparação para ordenação dos arrays acima:
    bool compareByMoves(playerData a, playerData b);
    bool compareByTime(playerData a, playerData b);

signals:
    void changedHOFContents(); //atualizar conteúdo do Hall da Fama
    void changedUpdateBoard(); //atualizar textos dos botões

};

#endif // GAME_H
