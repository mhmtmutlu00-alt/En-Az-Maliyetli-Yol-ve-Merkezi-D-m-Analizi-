#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <windows.h>

#define V_COUNT 6
#define INF 1000000000

/* ANSI Renk Kodlari */
#define RESET       L"\033[0m"
#define BOLD        L"\033[1m"
#define CYAN        L"\033[36m"
#define MAGENTA     L"\033[35m"
#define GREEN       L"\033[32m"
#define YELLOW      L"\033[33m"
#define RED         L"\033[31m"
#define CLEAR_SCR   L"\033[H\033[J"

static const char NODE_NAMES[V_COUNT] = {'A', 'B', 'C', 'D', 'E', 'F'};

int nodeIndex(char ch) {
    for (int i = 0; i < V_COUNT; i++) {
        if (NODE_NAMES[i] == ch) return i;
    }
    return -1;
}

void addUndirectedEdge(int graph[V_COUNT][V_COUNT], char uCh, char vCh, int cost) {
    int u = nodeIndex(uCh);
    int v = nodeIndex(vCh);
    if (u != -1 && v != -1) {
        graph[u][v] = cost;
        graph[v][u] = cost;
    }
}

/* 1. Gorev: Verilen kok dugum icin adim adim Dijkstra algoritmasi */
void runDijkstra(int graph[V_COUNT][V_COUNT], int src, int printSteps,
                 int dist[V_COUNT], int parent[V_COUNT], int known[V_COUNT]) {
    for (int i = 0; i < V_COUNT; i++) {
        dist[i] = INF;
        parent[i] = -1;
        known[i] = 0;
    }
    dist[src] = 0;

    if (printSteps) {
        wprintf(BOLD CYAN L"\n=== 1. GOREV: Kok Dugum %c Icin Adim Adim Dijkstra Tablosu ===\n" RESET,
                NODE_NAMES[src]);
    }

    for (int step = 0; step < V_COUNT; step++) {
        int u = -1;
        int minDist = INF;

        for (int i = 0; i < V_COUNT; i++) {
            if (!known[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        if (u == -1) break;
        known[u] = 1;

        for (int v = 0; v < V_COUNT; v++) {
            if (!known[v] && graph[u][v] != INF && dist[u] != INF) {
                if (dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = dist[u] + graph[u][v];
                    parent[v] = u;
                }
            }
        }

        if (printSteps) {
            wprintf(BOLD YELLOW L"Adim %d (Secilen: %c): " RESET, step + 1, NODE_NAMES[u]);
            for (int i = 0; i < V_COUNT; i++) {
                char pCh = (parent[i] == -1) ? '-' : NODE_NAMES[parent[i]];
                if (dist[i] == INF) {
                    wprintf(L"[%c: K=%d, D=INF, Y=%c] ", NODE_NAMES[i], known[i], pCh);
                } else {
                    wprintf(L"[%c: K=%d, D=%2d, Y=%c] ", NODE_NAMES[i], known[i], dist[i], pCh);
                }
            }
            wprintf(L"\n");
        }
    }

    if (printSteps) {
        wprintf(BOLD GREEN L"\nFinal Dijkstra Tablosu (Kok: %c):\n" RESET, NODE_NAMES[src]);
        wprintf(L"  Dugum | Isaret (Known) | Maliyet (Dist) | Yol / Onceki (Path)\n");
        wprintf(L"  ------+----------------+----------------+--------------------\n");
        for (int i = 0; i < V_COUNT; i++) {
            char pCh = (parent[i] == -1) ? '-' : NODE_NAMES[parent[i]];
            wprintf(L"    %c   |       %d        |      %2d        |         %c\n",
                    NODE_NAMES[i], known[i], dist[i], pCh);
        }
    }
}

/* Dikdortgen dagitim merkezleri ve aktif rota adimini animasyonlu cizer */
void renderRouteAnimation(const wchar_t *title, const int path[], int pathLen,
                          int activePos, const int dist[V_COUNT], int graph[V_COUNT][V_COUNT]) {
    /* Sol dikdortgen: {A, B, C}, Sag dikdortgen: {D, E, F} */
    const int leftSet[3] = {0, 1, 2};
    const int rightSet[3] = {3, 4, 5};

    int activeNode = (activePos >= 0 && activePos < pathLen) ? path[activePos] : -1;

    wprintf(CLEAR_SCR);
    wprintf(BOLD YELLOW L"=== %ls ===\n\n" RESET, title);
    wprintf(BOLD CYAN L"    BOLGE 1 MERKEZLERI            ROTA ANIMASYONU            BOLGE 2 MERKEZLERI\n" RESET);
    wprintf(CYAN L"  ┌──────────────────────┐                                 ┌──────────────────────┐\n" RESET);

    for (int r = 0; r < 3; r++) {
        int lIdx = leftSet[r];
        int rIdx = rightSet[r];

        /* Sol Dikdortgen */
        if (lIdx == activeNode) {
            wprintf(CYAN L"  │ " BOLD GREEN L"> Merkez %c (d:%2d) <" RESET CYAN L" │" RESET,
                    NODE_NAMES[lIdx], dist[lIdx]);
        } else {
            wprintf(CYAN L"  │   Merkez %c (d:%2d)   │" RESET,
                    NODE_NAMES[lIdx], dist[lIdx]);
        }

        /* Orta Alan: Aktif adim veya toplam maliyet */
        if (r == 1) {
            if (activePos >= 0 && activePos < pathLen - 1) {
                int u = path[activePos];
                int v = path[activePos + 1];
                wprintf(BOLD YELLOW L"   ──(%c)─[%d]─►(%c)──   " RESET,
                        NODE_NAMES[u], graph[u][v], NODE_NAMES[v]);
            } else {
                int target = path[pathLen - 1];
                wprintf(BOLD GREEN L"   ◄══ MALIYET: %2d ══►   " RESET, dist[target]);
            }
        } else {
            wprintf(L"                                 ");
        }

        /* Sag Dikdortgen */
        if (rIdx == activeNode) {
            wprintf(MAGENTA L"│ " BOLD GREEN L"> Merkez %c (d:%2d) <" RESET MAGENTA L" │\n" RESET,
                    NODE_NAMES[rIdx], dist[rIdx]);
        } else {
            wprintf(MAGENTA L"│   Merkez %c (d:%2d)   │\n" RESET,
                    NODE_NAMES[rIdx], dist[rIdx]);
        }

        if (r < 2) {
            wprintf(CYAN L"  │                      │" RESET L"                                 " MAGENTA L"│                      │\n" RESET);
        }
    }

    wprintf(CYAN L"  └──────────────────────┘                                 └──────────────────────┘\n\n" RESET);

    /* Rota ilerleme seridi */
    wprintf(BOLD L"Rota Izleme: " RESET);
    for (int i = 0; i < pathLen; i++) {
        if (i == activePos) {
            wprintf(BOLD YELLOW L"[ %c ]" RESET, NODE_NAMES[path[i]]);
        } else if (i < activePos || activePos == -1) {
            wprintf(BOLD GREEN L"%c" RESET, NODE_NAMES[path[i]]);
        } else {
            wprintf(L"%c", NODE_NAMES[path[i]]);
        }
        if (i < pathLen - 1) wprintf(L" -> ");
    }
    wprintf(L"\n");
    fflush(stdout);
}

/* 2. Gorev: Tersten okuma yontemiyle rotayi cikarir, animasyonla gosterir ve yazdirir */
void traceAnimateAndPrintRoute(int src, int target, const int parent[V_COUNT],
                               const int dist[V_COUNT], int graph[V_COUNT][V_COUNT],
                               const wchar_t *animTitle) {
    int path[V_COUNT];
    int pathLen = 0;
    int cur = target;

    /* Tersten okuma (hedeften koke) */
    while (cur != -1) {
        path[pathLen++] = cur;
        cur = parent[cur];
    }

    /* Diziyi kok -> hedef sirasina cevir */
    for (int i = 0; i < pathLen / 2; i++) {
        int tmp = path[i];
        path[i] = path[pathLen - 1 - i];
        path[pathLen - 1 - i] = tmp;
    }

    /* Animasyon kareleri */
    for (int step = 0; step < pathLen; step++) {
        renderRouteAnimation(animTitle, path, pathLen, step, dist, graph);
        Sleep(420);
    }
    renderRouteAnimation(animTitle, path, pathLen, -1, dist, graph);
    Sleep(700);
}

void printRouteDetailsText(int src, int target, const int parent[V_COUNT], const int dist[V_COUNT]) {
    int path[V_COUNT];
    int len = 0;
    int cur = target;

    wprintf(BOLD CYAN L"  %c -> %c En Dusuk Maliyet: " BOLD YELLOW L"%d\n" RESET,
            NODE_NAMES[src], NODE_NAMES[target], dist[target]);

    wprintf(L"    Tersten Okuma (Hedeften Koke): ");
    while (cur != -1) {
        wprintf(L"%c", NODE_NAMES[cur]);
        path[len++] = cur;
        cur = parent[cur];
        if (cur != -1) wprintf(L" <- ");
    }

    wprintf(L"\n    Duz Rota (Kokten Hedefe)     : " BOLD GREEN);
    for (int i = len - 1; i >= 0; i--) {
        wprintf(L"%c%ls", NODE_NAMES[path[i]], (i > 0) ? L" -> " : L"");
    }
    wprintf(RESET L"\n");
}

int computeSptCost(int src, const int parent[V_COUNT], int graph[V_COUNT][V_COUNT]) {
    int total = 0;
    for (int i = 0; i < V_COUNT; i++) {
        if (i != src && parent[i] != -1) {
            total += graph[parent[i]][i];
        }
    }
    return total;
}

int main(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
    setlocale(LC_ALL, "");

    int graph[V_COUNT][V_COUNT];
    for (int i = 0; i < V_COUNT; i++) {
        for (int j = 0; j < V_COUNT; j++) {
            graph[i][j] = (i == j) ? 0 : INF;
        }
    }

    /* Girdi tablosundaki yonsuz agirlikli kenarlar */
    addUndirectedEdge(graph, 'A', 'B', 7);
    addUndirectedEdge(graph, 'A', 'C', 5);
    addUndirectedEdge(graph, 'A', 'F', 1);
    addUndirectedEdge(graph, 'B', 'C', 1);
    addUndirectedEdge(graph, 'B', 'D', 1);
    addUndirectedEdge(graph, 'B', 'E', 3);
    addUndirectedEdge(graph, 'C', 'D', 2);
    addUndirectedEdge(graph, 'C', 'E', 1);
    addUndirectedEdge(graph, 'D', 'F', 7);
    addUndirectedEdge(graph, 'E', 'F', 2);

    /* Kullanicidan kok dugum alma */
    char inputCh = 'A';
    wprintf(BOLD YELLOW L"=== SORU 2: EN AZ MALIYETLI YOL VE MERKEZI DUGUM ANALIZI (DIJKSTRA) ===\n\n" RESET);
    wprintf(L"1. Gorev icin kok dugum giriniz (A, B, C, D, E, F): ");
    fflush(stdout);

    if (scanf(" %c", &inputCh) == 1) {
        if (inputCh >= 'a' && inputCh <= 'f') {
            inputCh = (char)(inputCh - 'a' + 'A');
        }
    }
    int userSrc = nodeIndex(inputCh);
    if (userSrc == -1) {
        wprintf(RED L"Gecersiz giris! Varsayilan kok dugum A secildi.\n" RESET);
        userSrc = 0;
    }

    /* 2. Gorev icin A kokunden Dijkstra calistir ve animasyonla A->B ve A->D rotalarini goster */
    int distA[V_COUNT], parentA[V_COUNT], knownA[V_COUNT];
    runDijkstra(graph, nodeIndex('A'), 0, distA, parentA, knownA);

    traceAnimateAndPrintRoute(nodeIndex('A'), nodeIndex('B'), parentA, distA, graph,
                              L"2. GOREV ANIMASYONU: KOK A -> B EN KISA ROTA");
    traceAnimateAndPrintRoute(nodeIndex('A'), nodeIndex('D'), parentA, distA, graph,
                              L"2. GOREV ANIMASYONU: KOK A -> D EN KISA ROTA");

    /* Ekrani temizleyip tum 3 gorevin raporunu tek ekranda net yazdir */
    wprintf(CLEAR_SCR);
    wprintf(BOLD YELLOW L"=== SORU 2: TUM GOREVLERIN SONUC RAPORU ===\n" RESET);

    /* 1. Gorev Ciktisi */
    int distUser[V_COUNT], parentUser[V_COUNT], knownUser[V_COUNT];
    runDijkstra(graph, userSrc, 1, distUser, parentUser, knownUser);

    /* 2. Gorev Ciktisi */
    wprintf(BOLD CYAN L"\n=== 2. GOREV: Kok Dugum A Icin B ve D Rotalari (Tersten Okuma) ===\n" RESET);
    printRouteDetailsText(nodeIndex('A'), nodeIndex('B'), parentA, distA);
    printRouteDetailsText(nodeIndex('A'), nodeIndex('D'), parentA, distA);

    /* 3. Gorev Ciktisi: Tum dugumleri (A-F) kok kabul ederek merkezi dugum tespiti */
    wprintf(BOLD CYAN L"\n=== 3. GOREV: Merkezi Dugum Tespiti (Tum Dugumler Kok) ===\n" RESET);
    int totalDistSum[V_COUNT];
    int sptEdgeSum[V_COUNT];
    int minTotalDist = INF;

    for (int r = 0; r < V_COUNT; r++) {
        int d[V_COUNT], p[V_COUNT], k[V_COUNT];
        runDijkstra(graph, r, 0, d, p, k);

        int sumD = 0;
        for (int i = 0; i < V_COUNT; i++) {
            sumD += d[i];
        }
        totalDistSum[r] = sumD;
        sptEdgeSum[r] = computeSptCost(r, p, graph);

        if (sumD < minTotalDist) {
            minTotalDist = sumD;
        }
    }

    wprintf(L"  Kok | En Kisa Yollar Toplami | En Kisa Yol Agaci (SPT) Maliyeti\n");
    wprintf(L"  ----+------------------------+---------------------------------\n");
    for (int r = 0; r < V_COUNT; r++) {
        if (totalDistSum[r] == minTotalDist) {
            wprintf(BOLD GREEN L"   %c  |          %2d            |                %2d   <-- EN MERKEZI\n" RESET,
                    NODE_NAMES[r], totalDistSum[r], sptEdgeSum[r]);
        } else {
            wprintf(L"   %c  |          %2d            |                %2d\n",
                    NODE_NAMES[r], totalDistSum[r], sptEdgeSum[r]);
        }
    }

    wprintf(BOLD GREEN L"\nSonuc: En dusuk toplam maliyete (%d) sahip en merkezi dugum(ler): " BOLD YELLOW, minTotalDist);
    int first = 1;
    for (int r = 0; r < V_COUNT; r++) {
        if (totalDistSum[r] == minTotalDist) {
            wprintf(L"%ls%c", first ? L"" : L", ", NODE_NAMES[r]);
            first = 0;
        }
    }
    wprintf(RESET L"\n");

    wprintf(BOLD L"\nCikmak icin Enter'a basin...\n" RESET);
    getwchar();
    getwchar();
    return 0;
}