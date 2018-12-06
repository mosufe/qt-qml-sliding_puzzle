import QtQuick 2.9
import QtQuick.Window 2.2
import CustomComponents 1.0

/*
*******************************************************************************
Window
A janela principal. Guarda todos os elementos gráficos do programa como filhos.
*******************************************************************************
*/
Window {
    visible: true
    width: 640
    height: 340
    title: qsTr("Sliding Puzzle")
    color: "black"
    Game{
        /*
        *******************************************************************************
        Game <id: theGame>
        Classe C++ contendo a lógica do jogo, registrada neste arquivo QML. Reage sobre
        os elementos gráficos do programa: as peças do quebra cabeça e o Hall da Fama.
        *******************************************************************************
        */
        id: theGame
        onHOFChangeChanged: {
            /*
            Na classe C++, a QString hOFChange contém o texto do Hall da fama, que é gerado em
            C++ e exibido aqui.
            */
            hallOfFameField.text = hOFChange;
        }
        onChangedUpdateBoard: {
            /*
            A classe C++ Game guarda uma matriz com todos os textos que serão exibidos na
            tela da interface (o "tabuleiro" do jogo). Esses textos podem corresponder
            tanto aos números das peças quanto à mensagem "CONGRATULATIONS!" exibida quando
            termina o jogo.
            */
            tiles.itemAt(0).tileX = getTileTexts(0);
            tiles.itemAt(1).tileX = getTileTexts(1);
            tiles.itemAt(2).tileX = getTileTexts(2);
            tiles.itemAt(3).tileX = getTileTexts(3);
            tiles.itemAt(4).tileX = getTileTexts(4);
            tiles.itemAt(5).tileX = getTileTexts(5);
            tiles.itemAt(6).tileX = getTileTexts(6);
            tiles.itemAt(7).tileX = getTileTexts(7);
            tiles.itemAt(8).tileX = getTileTexts(8);
            tiles.itemAt(9).tileX = getTileTexts(9);
            tiles.itemAt(10).tileX = getTileTexts(10);
            tiles.itemAt(11).tileX = getTileTexts(11);
            tiles.itemAt(12).tileX = getTileTexts(12);
            tiles.itemAt(13).tileX = getTileTexts(13);
            tiles.itemAt(14).tileX = getTileTexts(14);
            tiles.itemAt(15).tileX = getTileTexts(15);
            startGameArea.text = getStartGameBtnText();
        }

    }

    Text{
        /*
        *******************************************************************************
        Text
        Área reservada à entrada de usuário, para que este forneça o seu nome ao
        programa. Ao término de uma partida, o texto no item filho nameField será
        utilizado para inclusão no Hall da Fama.
        *******************************************************************************
        */
        x:10
        color: "gold"
        text: "Name: "
        font.pointSize: 20
        TextInput{
            /*
            *******************************************************************************
            TextInput <id: nameField>
            O único TextInput do jogo, portanto sempre terá o foco. Aceita a entrada de
            teclado para que o usuário forneça seu nome
            *******************************************************************************
            */
            id: nameField
            color: focus? "white":"gray"
            focus: true
            font.pointSize: 20
            maximumLength: 11
            x:100
            y:0
            text: "Your name"
        }

        Rectangle{
            /*
            *******************************************************************************
            Rectangle
            Retângulo reservado à área das peças do jogo. Contém como filhos todas as peças
            do jogo.
            *******************************************************************************
            */
            y: 30
            width: parent
            height: parent
            Grid {
                x:5; y:5; rows:4; columns:4; spacing:5;
                Repeater{
                    /*
                    *******************************************************************************
                    Repeater <id: tiles>
                    Repetidor que constrói em grade todas as 16 peças de jogo como novos quadrados
                    e uma área de texto para receber as mensagens (números das peças ou a palavra
                    "CONGRATULATIONS").
                    *******************************************************************************
                    */
                    id: tiles
                    model: 16
                    Rectangle{
                        /*
                        *******************************************************************************
                        Rectangle
                        As peças do jogo, programadas para aparecerem na mesma cor do fundo (preto)
                        caso contenham o número 16 que, neste jogo, representa o espaço vazio. Caso
                        contrário, ela será vermelha viva quando em contato com o cursor do mouse e
                        vermelha escura caso contrário. Elas começam todas com o texto "-", indicando
                        que nenhum jogo começou desde o início do programa. Sua propriedade tileX
                        é sincronizada com a classe Game e contém o texto a ser exibido em cada peça.
                        *******************************************************************************
                        */
                        width:70; height:70
                        color: tileTextArea.text=="16"? "black" : (mouseArea.containsMouse? "red":"darkred")
                        property string tileX: "-";
                        MouseArea{
                            /*
                            *******************************************************************************
                            MouseArea <id: mouseArea>
                            Estende-se por toda a dimensão do seu parente, a peça. Realiza a mudança da cor
                            de uma peça diferente da 16 de vermelho escuro para vermelho vivo e aciona o
                            slot de botão clicado na classe Game (theGame.buttonClickedOnDisplay(index)).
                            *******************************************************************************
                            */
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                theGame.buttonClickedOnDisplay(index);
                            }
                        }

                        Text{
                            /*
                            *******************************************************************************
                            Text <id: tileTextArea>
                            Contém de fato o texto a ser exibido na peça. Aparece na cor preta para se
                            misturar ao seu fundo caso contenha o valor 16 (representando uma área vazia)
                            e a cor ouro caso contrário. Obtém seus valores da propriedade tileX do
                            retângulo pai.
                            *******************************************************************************
                            */
                            id: tileTextArea
                            text: parent.tileX
                            color: text=="16"? "black":"gold"
                            font.pointSize: 30
                            anchors.centerIn: parent
                        }
                    }
                }
            }
            Rectangle{
                /*
                *******************************************************************************
                Rectangle
                Retângulo correspondente ao início do jogo. Aparece na cor cinza claro até que
                o cursor do mouse entre em contato com ele, quando passará a ser branca clara.
                Contém um texto explicativo e uma área de mouse. Ao clicar sobre a área de
                mouse, caso exista um jogo em execução, ele será interrompido e um novo começará,
                pelo acionamento do slot theGame.startGame() da classe Game.
                Além disso, é acionado o slot getPlayerName(nameField.text) da classe Game,
                que recebe como parâmetro o nome do jogador (nameField.text). Com isso, o jogo
                estará preparado para usar o nome do jogador no Hall da Fama.
                *******************************************************************************
                */
                x:310
                y:240
                width: 300
                height: 60
                color: startGameMouseArea.containsMouse? "white":"lightgray"
                MouseArea{
                    /*
                    *******************************************************************************
                    MouseArea <id: startGameMouseArea>
                    A área do mouse se estende sobre todo o retângulo. O acionamento deste botão
                    é semelhante ao caso das peças do jogo.
                    *******************************************************************************
                    */
                    id: startGameMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        theGame.getPlayerName(nameField.text);
                        theGame.startGame();
                    }
                }
                Text{
                    /*
                    *******************************************************************************
                    Text <id: startGameArea>
                    Texto do botão de começar jogo. Será exibido sempre na cor preta.
                    *******************************************************************************
                    */
                    id: startGameArea
                    color:"black"
                    font.pointSize: 25
                    text:"START NEW GAME"
                    anchors.centerIn: parent
                    lineHeight: 0.8
                }
            }
        Text{
            /*
            *******************************************************************************
            Text
            Texto do rótulo do Hall da Fama. Exibe o título Hall da Fama na cor ouro e em
            fonte sublinhada e em negrito.
            *******************************************************************************
            */
            color:"gold"
            font.pointSize: 15
            text: "Hall of Fame"
            font.underline: true
            font.bold: true
            x:340
            y:-20

            Text{
                /*
                *******************************************************************************
                Text <id: hallOfFameField>
                Área destinada à exibição do texto do Hall da Fama, que será atualizado pela
                classe C++.
                *******************************************************************************
                */
                id: hallOfFameField
                y:30
                color:"white"
                font.bold: true
                font.pointSize: 7.3
                lineHeight: 0.8
                text: "Enter your name and press\nSTART GAME button.\n\nEnter your name BEFORE \nstarting the game."
            }
        }
        }
    }
}
