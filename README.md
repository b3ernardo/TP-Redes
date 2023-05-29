<h3>Trabalho Prático 1 - Redes de Computadores</h3>
<p>Universidade Federal de Minas Gerais</p>

##

<div>
  </br>
  <p>O presente trabalho tem o objetivo de implementar um sistema de transferência de arquivos, entre um servidor e um cliente, utilizando sockets em linguagem C. A ideia é que um computador funcione como servidor e seja capaz de aceitar conexão a um cliente, receber arquivos do cliente e armazená-los no sistema (no caso, uma pasta do servidor), de modo que confirme o recebimento ao cliente e também seja capaz de receber o pedido de encerramento da conexão. O outro computador funciona como cliente, e é capaz de conectar-se ao servidor, selecionar um arquivo disponível, receber confirmação de recebimento e pedir o encerramento da conexão.</p>
  <p>Para fins de teste, utiliza-se dois terminais do prompt de comando, um que serve como servidor, e outro que serve como cliente. Na versão deste trabalho, os arquivos enviados são texto puro, aceitos nas extensões *.txt, *.c, *.cpp, *.py, *.tex e *.java. Cliente e servidor trocam mensagens de comando curtas (até 500 bytes), via protocolo TCP. Essas mensagens não aceitam caracteres acentuados e caracteres especiais. Todas as mensagens são inseridas no cliente, e a resposta vem do servidor.</p>
  <p>O cliente seleciona um arquivo e envia ao servidor. O servidor cria uma pasta chamada server_files contendo o arquivo. Toda vez que o servidor é ativado, essa pasta é apagada para começar o processo novamente. O conteúdo do arquivo que chega ao servidor possui um cabeçalho com seu nome na primeira linha, e o conteúdo restante (apenas com caracteres aceitos).</p>
  <p>Por fim, vale ressaltar que as comunicações são feitas com IPv4 ou IPv6, bastando escolher no prompt de comando.</p>

  <h3>Em dois terminais diferentes, rode:</h3>
  <p>./server v4 51511</p>
  <p>./client 127.0.0.1 51511</p>
  
  <h3>Comandos disponíveis:</h3>
  <p>select file [nome_do_arquivo]</p>
  <p>send file</p>
  <p>exit</p>
</div>
