# 🇮🇹 Boz OS

> Um sistema operacional i386 bare-metal independente, desenvolvido como um fork customizado do SaguiOS.

## 📖 Origem e História

O nome **Boz** carrega uma profunda herança histórica e familiar. O sobrenome (originalmente grafado *Boz*) foi trazido da Itália há muitos anos durante o grande fluxo de imigração italiana para o Brasil, fixando-se inicialmente no Rio Grande do Sul, na histórica região da **Colônia Caxias**. 

Em homenagem a essa raiz cultural, o **Boz OS** exibe orgulhosamente a bandeira da Itália em sua interface gráfica, unindo a paixão por sistemas operacionais de baixo nível à preservação da memória e da ancestralidade familiar.

---

## 🖼️ Capturas de Tela do Sistema

Aqui está o **Boz OS** em execução (rodando via emulador x86), mostrando o ambiente gráfico customizado, o gerenciamento de processos (`ps`), o sistema de arquivos e a bandeira italiana integrada à área de trabalho:

### Execução de Processos (`ps`) no Terminal
![Boz OS - Terminal e Processos](images/1000047846.jpg)

### Exploração do Sistema de Arquivos (`ls`)
![Boz OS - Estrutura de Arquivos](images/1000047715.jpg)

---

## ⚙️ Características Técnicas

- **Arquitetura:** i386 (32-bit x86 bare-metal).
- **Base:** Fork e rebrand customizado a partir do ecossistema **SaguiOS**.
- **Kernel:** Monolítico próprio com suporte a gerenciamento de processos, VFS, sistema de arquivos SGFS e interface gráfica em modo protegido.
- **Ambiente Gráfico:** Janelas gerenciadas, barra de tarefas, ícones customizados e terminal integrado (`sagsh`).
- **Compilação Cruzada:** Desenvolvido e compilado utilizando a toolchain GNU (`i686-linux-gnu-`) e scripts de automação em Python.

---

## 🚀 Como Compilar e Executar

Certifique-se de possuir a toolchain cruzada do GCC para i386, `nasm` e `python3` instalados no ambiente (como o Termux ou Linux nativo).

1. Clone o repositório:
   ```bash
   git clone [https://github.com/Daviboss76/Boz-os.git](https://github.com/Daviboss76/Boz-os.git)
   cd Boz-os

