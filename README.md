# ProjectModernCpp

The Game - Modern C++ Server ImplementationAceasta este implementarea server-side a popularului joc de cărți "The Game", construită folosind standardele moderne C++20. Proiectul a trecut printr-un proces riguros de refactorizare pentru a asigura o arhitectură curată, încapsulare și testabilitate. 

Arhitectură și DesignServerul este organizat în straturi decuplate pentru a facilita mentenanța și scalabilitatea:Modele de Date (Entities): Clase precum GameEntity, PlayerParticipant și User folosesc încapsularea totală. 

Membrii sunt private, accesul fiind permis doar prin metode de tip Getter și Setter.Stratul de Servicii (Logic):MatchService: Gestionează logica de matchmaking (bazată pe scorul de performanță) și inițializarea meciurilor.UserService: Responsabil pentru autentificare (JWT-like tokens), înregistrare și calcularea dinamică a statisticilor jucătorilor.

Persistență (Database): Utilizarea sqlite_orm permite o mapare obiect-relațională (ORM) declarativă, eliminând SQL-ul scris manual și asigurând siguranța tipurilor.Comunicare: Integrare cu framework-ul Crow pentru WebSocket-uri și managementul sesiunilor.Tehnologii UtilizateTehnologieScopC++20Limbajul de programare principal, utilizând smart pointers și move semantics.

CrowFramework web pentru API-uri REST și comunicare bidirecțională prin WebSockets.sqlite_ormGestionarea bazei de date SQLite printr-o interfață modernă de C++.
Google Test (GTest)Framework pentru Unit Testing și asigurarea calității codului.
ProtobufProtocol binar pentru serializarea rapidă a stării jocului către clienți.

Unit Testing și CalitateProiectul pune un accent deosebit pe testare, având o acoperire (coverage) de peste 50% pe întreg codul de server. 
Testele verifică:Logica pură a jocului: Validarea mutărilor pe stivele ascendente/descendente și regulile speciale (Backjump -10).
Algoritmi de Matchmaking: Verificarea intervalelor de scor corecte pentru gruparea jucătorilor.Integritatea Datelor: Testarea corectitudinii distribuției cărților și rotației jucătorilor.

Instalare și RulareCerințe:Visual Studio 2022 (cu suport C++20).vcpkg pentru gestionarea dependențelor (crow, sqlite_orm, protobuf).
Build:Clonează repository-ul.Deschide fișierul .sln în Visual Studio.
Asigură-te că proiectul TheGameTests este setat pentru a rula în modul Debug/x64.Rulează Test Explorer pentru a verifica integritatea logică.