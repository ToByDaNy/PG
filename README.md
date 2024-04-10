To run the project run the .exe executable.

Cuprins:
1. Cuprins
2. Prezentarea temei
3. Scenariul
    3.1. descrierea scenei și a obiectelor
    3.2. funcționalități
4. Detalii de implementare
    4.1. funcții și algortmi
           4.1.1. soluții posibile
           4.1.2. motivarea abordării alese
    4.2. modelul grafic
    4.3. structuri de date
    4.4. ierarhia de clase
5. Prezentarea interfeței grafice utilizator / manual de utilizare
6. Concluzii și dezvoltări ulterioare
7. Referințe

2.	Prezentarea temei
Tema reprezinta o scena in natura intre munti brazi si rau, cu anumite animatii, ceata lumini si functionalitati. Proiect OpenGL cu efecte de iluminare avansate, modele dinamice și control al camerei. Include shadere, FBO și un skybox pentru realism.
3.	Scenariul
a.	Descrierea scenei si a obiectelor
O scena in natura intre munti, brazi si rau, cu anumite animatii, ceata, lumini, umbre. Cu 2 case si masini cu generare dinamica, un elicopter.
b.	Functionalitati
Miscarea obiectelor(rotirea ( q si e) si scalarea(r) nanosuitului, miscarea a masini de pe sosea si a 3 a masina generata dynamic(sageata sus si jos), animatie a masini intrand pe sosea(automata), animatia elicei elicopterului(automata), schimbarea directiei lumini directionale(infinite)/umbrei avand ca luna ca lumina)( j si l). Vizualizare scenă în modurile solid(z), wireframe(x), polygonal(c) și smooth(v). Schimbarea luminii punctiforme( 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, =, p – vizualizare schimbare in functie ) “ambient diffuse specular quadratic linear constant”, schimbarea intensitatii luminii directionale(b, n si TAB), vizualizarea mapei de umbre(m). Ceață (f apare, g dispare). Schimbarea camerei (WASD + miscare mouse), si schimbarea vitezei camerei(CAPS). Generare dinamica masini(t). Automatizarea prezentarii (Space). Animatie rain.Aprinderea si stingerea luminii punctiforme( comma “,” si backspace “\b”)
4.	Detalii de implementare
a.	Functii si algoritmi
i.	Solutii posibile
computeLightComponents() : Calculează componentele iluminate pentru sursa de lumină direcțională. Algoritmul in sine calculeaza o lumina infinita de la pozitia lui cu o anumita directie, razele fiind paralele. Acest algoritm calculeaza 3 coeficienti ambient, speculara si diffuse, acesti 3 coeficienti pentru lumina directionala sunt astfel :
-	Ambientul este calculat prin înmulțirea culorii luminii cu un scalar ce reprezintă puterea difuziei ambiante.
-	Diffuse calculează cât de multă lumină este dispersată uniform pe suprafața obiectului, luând în considerare unghiul dintre normala obiectului și direcția luminii. Se folosește max(0, dot(normala_obiect, normala_directie_lumina)) pentru a asigura că lumina este distribuită doar pe suprafețele care sunt orientate spre sursa de lumină.
-	Speculara calculează lumina reflectată de pe suprafața obiectului, având în vedere unghiul de lovire al suprafeței. Se utilizează un coeficient de putere (exponent) pentru a controla intensitatea și strălucirea acestei lumini reflectate.

computePointLight () : Calculeaza componentele illuminate de sursa de lumina punctiforma. Acest algoritm adauga la cei 3 coeficienti din lumina directionala o lumina locala. Calculand astfel:
-	Ambientul se calculeaza ca si in directionala cu un scalar dat de lumina locala si culoarea texturii de pe suprafata.
-	Diffuse aici are un scalar impartita cu o formula de calcul pentru distanta ca sa atenueze lumina cu cata distanta e parcursa inmultita cu textura de pe suprafata.
-	Speculara e la fel ca la directionala doar ca in loc de lumina e pusa culoarea texture obiectului
Iar la final e inmultita cu textura de pre suprafata obiectelor pentru a da culoare obiectelor;
	computeShadow(): calcularea umbelor se face prin calcularea unei matrici de adancime. In cod functia computeLightSpaceTrMatrix() calculeaza o matrice de adancime cu ajutorul directiei luminii. Aceasta matrice este transmisa in shader care calculeaza fiecare coordonata z daca e mai mare pe suprafata decat coordonata z din matricea de adancime.
Culoarea este calculata prin min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f) daca e numar mai mare ca 1 sa nu se calculeze ca culoare. Si cu cat umbra este mai mare atat nu se face lumina speculara sau diffuza, ambientul ajutand la evitarea umbrelor complete și la păstrarea detaliilor vizibile în zonele întunecate ale unei scenei.
		computeFog(): este o functie care calculeaza ceata. Ceata e calculata exponential prin distanta data de obiect fata pozitia camerei inmultita cu un scalar al cetii care reprezinta densitatea cetii(fog) : exp(-pow(fragmentDistance * fog, 2));.
La final culoarea este calculata printr un mix din culoarea calculata de lumini si umbra cu culoarea cetii si cu factorul de ceata calculat in functie
1. Desenarea Scenei Naturale:
   - Utilizarea coordonatelor și dimensiunilor pentru plasarea scenei, masinilor, elicopterului etc.
   - Texturarea și iluminarea adecvată pentru a conferi realism.

2. Animatii și Efecte:
   - Implementarea animațiilor pentru mișcarea masinii și a elicei elicopterului.
   - Utilizarea shaderelor pentru efecte de ceață și lumini atmosferice.

3. Controlul Camerei:
   - Implementarea unui sistem de control al camerei pentru explorarea detaliată a scenei.
   - Integrarea input-ului de la utilizator pentru rotire, deplasare.

4. Efecte de Iluminare Avansate:
   - Utilizarea tehnici avansate de iluminare, cum ar fi iluminarea ambientală și speculară.
   - Integrarea efectelor de umbre pentru obiecte și teren(lumina directionala).

5. Modelare Dinamică:
   - Implementarea unui sistem de modelare dinamică pentru a adăugarea de obiecte în timp real.
ii.	Motivarea abordarii alese
Curiozitate.
b.	Modelul graphic
Pentru a realiza o scenă grafică realistă, se propune adoptarea unui model grafic care să includă:
- Detalii privind geometria și aspectul muntelui, bradului și râului.
- Utilizarea eficientă a texturilor pentru a adăuga realism elementelor din scenă.
- Integrarea unui skybox pentru a oferi fundalul corespunzător scenei.
c.	Structuri de date
Pentru a gestiona eficient datele și pentru a asigura o execuție optimă a proiectului, se vor utiliza următoarele structuri de date:
- Structuri pentru coordonate și dimensiuni ale luminii in shader.
- Structuri pentru texturi și materiale pentru un control detaliat al aspectului vizual.
- Eventuale structuri pentru gestionarea animațiilor și a stării dinamice a scenei.
d.	Ierarhia de clase
Ierarhia de clase va constitui scheletul proiectului, facilitând organizarea și extensibilitatea. Se propune o structură ierarhică care să includă:
- O clasă de bază pentru obiectele din scenă.
- Clase derivate pentru munte, brad, râu și alte elemente specifice.
- Posibile clase pentru efecte speciale, cum ar fi ceața sau iluminarea.
5.	Prezentarea interfeti grafice utilizator/manual de utilizare(functionalitati)
   
 
 
6.	Concluzii si dezvoltari ulterioare
Adăugarea coliziunilor.
Optimizări și îmbunătățiri: Sugerări privind modul în care proiectul poate fi optimizat sau îmbunătățit în viitor. Deschideți ușa pentru feedback din partea altor utilizatori sau colegi. Discutați despre posibilitatea colaborărilor viitoare sau modul în care proiectul ar putea fi extins cu contribuțiile altor dezvoltatori.
7.	Referinte
Facultate.
https://youtu.be/E64VK-qolUU?si=G5mtFPLNEjf-ofk0 – pentru lumina punctiforma.
