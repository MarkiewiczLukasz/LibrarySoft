# LibrarySoft

Desktopowa aplikacja Windows wspierająca codzienną pracę małej biblioteki. Program
umożliwia katalogowanie książek, zarządzanie wypożyczeniami oraz prowadzenie listy
czytelników w przejrzystym interfejsie opartym o klasyczne okna dialogowe Win32.

## Funkcjonalności

- **Katalog zasobów** – dodawanie, edycja i usuwanie pozycji wraz z informacjami o
  autorze, gatunku, liczbie egzemplarzy oraz lokalizacji na regale.
- **Obsługa wypożyczeń** – szybkie przekazywanie książek do użytkowników i ich zwroty,
  z automatycznym aktualizowaniem stanów magazynowych.
- **Rejestr czytelników** – przechowywanie kart bibliotecznych wraz z danymi osobowymi,
  możliwość edycji i usuwania rekordów.
- **Zapisywanie danych na dysku** – wszystkie listy zapisywane są w plikach tekstowych w
  katalogu `Data`, dzięki czemu aplikacja uruchomiona ponownie odtworzy wcześniejszy stan.

## Urochomienie z CMAKE, bez CLion
Będąc w katalogu źródłowym

- Konfiguracja
`cmake -S . -B build`

- Budowanie
`cmake --build build`

- Uruchomienie
`.\build\Debug\LibrarySoft.exe`

## Struktura projektu

- `main.c` – logika głównego okna dialogowego, obsługa przycisków i formularzy.
- `FormFunctions.c` – pomocnicze funkcje interfejsu użytkownika, w tym bezpieczne operacje
  na kontrolkach list i kombi.
- `Lists.c` – warstwa dostępu do danych (listy książek, wypożyczeń i czytelników) wraz z
  funkcjami zapisu/odczytu.
- `objects.c` – definicje struktur danych i serializacja rekordów do formatu plikowego.
- `res/` – zasoby interfejsu (dialogi, identyfikatory kontrolek, ikony).

## Wymagania

- Kompilator C obsługujący WinAPI (np. MSVC, MinGW-w64).
- CLion
- System Windows (aplikacja korzysta z Win32 API i standardowych kontrolek).

Powstały plik wykonywalny (`LibrarySoft.exe`) należy uruchamiać z podkatalogiem `Data`
znajdującym się obok niego – to w nim przechowywane są wszystkie pliki z listami.

## Dane przykładowe

Folder `DataSeed` zawiera startowy zestaw plików z książkami, wypożyczeniami i
czytelnikami. zawartość katalogu `DataSeed` kopiowana jest do lokalizacji
pliku wykonywalnego do folderu `Data` przed pierwszym uruchomieniem programu.
