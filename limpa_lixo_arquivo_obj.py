import sys

nome_arq_entrada= sys.argv[1]
nome_arq_saida= sys.argv[2]

arq= open(f"{nome_arq_entrada}", 'r')
arq_out= open(f"{nome_arq_saida}",'w')
for i in arq:
    i= i.replace("//","")
    arq_out.write(i)

arq.close()
arq_out.close()
