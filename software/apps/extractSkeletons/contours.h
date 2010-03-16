#define edgecount 55
#define maxlength 161

int length_edge[edgecount]={161,32,13,30,56,19,33,65,96,10,14,15,64,45,14,30,14,27,40,12,11,23,21,21,21,22,77,70,159,118,11,16,10,23,14,68,10,24,10,12,13,79,19,22,25,79,10,20,11,13,50,15,41,13,20};

int edgelist[edgecount][maxlength][2]= \
							/*edge1*/{{{161,530},{160,529},{159,528},{159,527},{158,526},{158,525},{157,524},{157,523},{157,522},{156,521},{156,520},{156,519},{155,518},{155,517},{154,516},{153,516},{152,515},{151,514},{150,513},{149,512},{148,512},{147,512},{146,511},{145,511},{144,511},{143,511},{142,510},{141,510},{140,509},{139,508},{138,508},{137,507},{136,507},{135,506},{134,505},{133,505},{132,504},{131,503},{130,503},{129,502},{128,502},{127,501},{126,501},{125,500},{124,500},{123,499},{122,499},{121,498},{121,497},{120,496},{120,495},{120,494},{119,493},{119,492},{118,491},{117,490},{116,490},{115,490},{114,489},{113,489},{112,489},{111,489},{110,489},{109,489},{108,489},{107,489},{106,488},{105,488},{104,488},{103,487},{102,487},{101,486},{100,486},{99,486},{98,486},{97,485},{96,485},{95,484},{94,483},{93,482},{93,481},{93,480},{92,479},{91,478},{90,477},{90,476},{89,475},{89,474},{88,473},{87,472},{86,472},{85,471},{84,471},{83,470},{82,470},{81,469},{80,468},{79,467},{78,466},{77,465},{76,464},{75,463},{74,462},{73,461},{72,460},{71,459},{70,458},{69,458},{68,457},{67,456},{66,455},{65,455},{64,454},{63,454},{62,453},{61,453},{60,452},{59,452},{58,451},{57,451},{56,450},{55,449},{54,449},{53,449},{52,448},{51,448},{50,448},{49,447},{48,446},{48,445},{47,444},{46,443},{45,442},{44,441},{43,441},{42,440},{41,440},{40,439},{39,438},{38,438},{37,438},{36,438},{35,437},{34,437},{33,436},{33,435},{32,434},{31,433},{30,433},{29,432},{28,432},{27,432},{26,431},{25,431},{24,431},{23,431},{22,430},{21,430},{20,430},{19,430},{18,431}},\
							/*edge2*/{{45,241},{44,240},{43,239},{43,238},{42,237},{41,236},{40,236},{39,235},{38,234},{37,233},{36,232},{35,231},{34,230},{33,230},{32,229},{31,228},{31,227},{30,226},{30,225},{29,224},{29,223},{28,222},{27,221},{26,220},{26,219},{25,218},{25,217},{24,216},{23,215},{22,214},{21,214},{20,213}},\
							/*edge3*/{{33,298},{32,297},{31,296},{30,296},{29,295},{28,294},{27,293},{26,292},{25,291},{24,290},{23,289},{23,288},{23,287}},\
							/*edge4*/{{47,227},{47,226},{47,225},{46,224},{46,223},{46,222},{46,221},{45,220},{44,219},{43,218},{42,218},{41,217},{40,216},{40,215},{40,214},{39,213},{39,212},{39,211},{38,210},{37,209},{36,209},{35,208},{34,208},{33,208},{32,208},{31,208},{30,208},{29,208},{28,208},{27,208}},\
							/*edge5*/{{84,354},{84,353},{84,352},{84,351},{83,350},{83,349},{82,348},{82,347},{82,346},{81,345},{81,344},{80,343},{79,342},{78,341},{78,340},{77,339},{76,339},{75,338},{74,338},{73,337},{72,336},{72,335},{71,334},{71,333},{70,332},{69,332},{68,331},{67,330},{66,329},{65,328},{64,327},{64,326},{63,325},{63,324},{62,323},{61,322},{60,321},{59,320},{58,319},{57,318},{56,317},{55,316},{54,315},{53,314},{52,313},{51,312},{50,311},{49,310},{48,310},{47,309},{46,308},{45,307},{44,306},{43,305},{42,304},{41,303}},\
							/*edge6*/{{65,257},{64,256},{63,255},{62,255},{61,254},{60,254},{59,253},{58,253},{57,252},{56,251},{55,251},{54,250},{53,249},{52,248},{51,247},{50,246},{49,245},{48,244},{47,243}},\
							/*edge7*/{{50,639},{50,638},{50,637},{51,636},{51,635},{51,634},{51,633},{52,632},{52,631},{53,630},{53,629},{54,628},{55,628},{56,627},{57,627},{58,626},{59,626},{60,626},{61,626},{62,625},{63,625},{64,625},{65,625},{66,625},{67,625},{68,625},{69,626},{70,626},{71,626},{72,627},{73,627},{74,627},{75,628}},\
							/*edge8*/{{65,274},{66,275},{67,276},{68,277},{69,278},{70,279},{71,280},{72,280},{73,281},{74,281},{75,281},{76,281},{77,280},{78,280},{79,279},{80,278},{81,277},{82,276},{82,275},{83,274},{84,273},{84,272},{85,271},{85,270},{85,269},{85,268},{85,267},{84,266},{84,265},{84,264},{83,263},{83,262},{82,261},{81,261},{80,260},{79,260},{78,259},{77,258},{76,257},{76,256},{76,255},{75,254},{75,253},{75,252},{75,251},{74,250},{74,249},{74,248},{73,247},{73,246},{72,245},{71,244},{70,243},{69,242},{69,241},{68,240},{68,239},{67,238},{67,237},{67,236},{66,235},{66,234},{65,233},{64,232},{63,231}},\
							/*edge9*/{{139,307},{138,307},{137,308},{136,308},{135,308},{134,308},{133,307},{132,307},{131,306},{131,305},{130,304},{129,303},{128,302},{128,301},{127,300},{127,299},{126,298},{126,297},{125,296},{124,296},{123,295},{122,294},{122,293},{121,292},{120,292},{119,291},{118,291},{117,290},{116,290},{115,289},{114,288},{113,288},{112,287},{112,286},{111,285},{110,284},{109,283},{108,282},{107,281},{106,280},{105,279},{104,278},{103,277},{103,276},{102,275},{101,274},{101,273},{100,272},{99,271},{99,270},{98,269},{97,268},{96,267},{96,266},{95,265},{95,264},{94,263},{94,262},{94,261},{94,260},{95,259},{96,258},{97,258},{98,258},{99,258},{100,258},{101,258},{102,259},{103,260},{104,261},{105,262},{106,263},{107,264},{108,265},{109,266},{110,266},{111,267},{112,268},{113,269},{114,270},{115,270},{116,271},{117,272},{118,273},{119,274},{120,274},{121,275},{122,276},{123,276},{124,277},{124,278},{125,279},{126,280},{126,281},{127,282},{128,283}},\
							/*edge10*/{{120,357},{119,358},{119,359},{118,360},{117,361},{116,361},{115,362},{114,362},{113,362},{112,363}},\
							/*edge11*/{{133,394},{132,394},{131,394},{130,394},{129,394},{128,394},{127,394},{126,393},{125,392},{124,391},{124,390},{124,389},{124,388},{124,387}},\
							/*edge12*/{{142,438},{141,439},{140,440},{139,440},{138,440},{137,440},{136,439},{135,439},{134,439},{133,438},{132,437},{131,436},{131,435},{130,434},{131,433}},\
							/*edge13*/{{131,346},{131,345},{132,344},{133,343},{134,342},{135,341},{136,341},{137,341},{138,341},{139,341},{140,342},{141,342},{142,343},{143,343},{144,344},{145,345},{145,346},{146,347},{147,348},{147,349},{148,350},{149,350},{150,351},{151,351},{152,352},{153,352},{154,353},{155,353},{156,354},{157,355},{158,356},{159,357},{160,358},{161,359},{161,360},{162,361},{163,362},{163,363},{164,364},{165,365},{166,366},{167,367},{168,368},{169,368},{170,368},{171,368},{172,367},{173,366},{173,365},{173,364},{174,363},{174,362},{175,361},{175,360},{176,359},{176,358},{176,357},{176,356},{175,355},{174,354},{174,353},{173,352},{172,351},{171,350},{171,349}},\
							/*edge14*/{{148,236},{148,235},{148,234},{148,233},{148,232},{148,231},{148,230},{148,229},{148,228},{148,227},{148,226},{148,225},{149,224},{149,223},{149,222},{149,221},{149,220},{149,219},{149,218},{149,217},{149,216},{149,215},{148,214},{148,213},{148,212},{147,211},{147,210},{146,209},{146,208},{145,207},{145,206},{144,205},{144,204},{144,203},{143,202},{142,201},{141,200},{140,200},{139,199},{138,199},{137,200},{136,200},{135,201},{134,201},{133,202}},\
							/*edge15*/{{150,403},{149,403},{148,403},{147,403},{146,403},{145,402},{144,402},{143,402},{142,402},{141,401},{140,401},{139,401},{138,401},{137,400}},\
							/*edge16*/{{157,271},{156,270},{155,270},{154,269},{153,268},{152,267},{152,266},{152,265},{152,264},{152,263},{152,262},{152,261},{152,260},{152,259},{152,258},{152,257},{151,256},{150,255},{149,254},{148,254},{147,253},{146,252},{145,251},{144,250},{143,249},{142,248},{141,248},{140,247},{139,247},{138,247}},\
							/*edge17*/{{160,404},{161,403},{161,402},{161,401},{161,400},{161,399},{160,398},{160,397},{159,396},{158,395},{157,395},{156,395},{155,394},{154,394}},\
							/*edge18*/{{172,329},{173,330},{174,330},{175,331},{176,332},{176,333},{176,334},{176,335},{175,336},{174,337},{173,338},{172,338},{171,338},{170,338},{169,338},{168,338},{167,338},{166,338},{165,338},{164,338},{163,338},{162,338},{161,339},{160,339},{159,339},{158,339},{157,339}},\
							/*edge19*/{{178,302},{177,301},{176,301},{175,300},{175,299},{174,298},{174,297},{174,296},{174,295},{174,294},{174,293},{175,292},{176,291},{177,290},{178,289},{179,289},{180,288},{180,287},{181,286},{181,285},{181,284},{181,283},{181,282},{181,281},{181,280},{180,279},{180,278},{179,277},{178,276},{177,275},{176,275},{175,274},{174,274},{173,274},{172,274},{171,273},{170,273},{169,273},{168,273},{167,273}},\
							/*edge20*/{{171,555},{172,554},{172,553},{172,552},{172,551},{172,550},{171,549},{171,548},{171,547},{170,546},{170,545},{169,544}},\
							/*edge21*/{{181,407},{181,408},{181,409},{181,410},{181,411},{181,412},{181,413},{181,414},{181,415},{181,416},{180,417}},\
							/*edge22*/{{197,231},{196,231},{195,230},{194,229},{193,228},{192,227},{192,226},{191,225},{190,224},{190,223},{190,222},{189,221},{188,220},{187,219},{186,219},{185,219},{184,219},{183,218},{182,217},{182,216},{181,215},{181,214},{181,213}},\
							/*edge23*/{{181,396},{181,395},{181,394},{182,393},{182,392},{182,391},{183,390},{183,389},{184,388},{183,387},{184,386},{184,385},{184,384},{184,383},{184,382},{184,381},{184,380},{185,379},{185,378},{185,377},{185,376}},\
							/*edge24*/{{183,427},{184,428},{184,429},{184,430},{184,431},{183,432},{183,433},{183,434},{183,435},{183,436},{183,437},{182,438},{182,439},{182,440},{182,441},{182,442},{182,443},{182,444},{182,445},{182,446},{181,447}},\
							/*edge25*/{{189,405},{190,406},{190,407},{190,408},{190,409},{189,410},{189,411},{188,412},{188,413},{188,414},{188,415},{188,416},{187,417},{187,418},{187,419},{186,420},{186,421},{186,422},{185,423},{185,424},{184,425}},\
							/*edge26*/{{186,365},{186,364},{186,363},{186,362},{186,361},{186,360},{186,359},{186,358},{186,357},{186,356},{186,355},{186,354},{186,353},{186,352},{186,351},{187,350},{187,349},{187,348},{187,347},{187,346},{187,345},{187,344}},\
							/*edge27*/{{187,400},{187,399},{187,398},{188,397},{188,396},{188,395},{188,394},{188,393},{188,392},{188,391},{189,390},{189,389},{189,388},{190,387},{190,386},{190,385},{190,384},{190,383},{190,382},{190,381},{191,380},{191,379},{191,378},{191,377},{191,376},{191,375},{192,374},{192,373},{192,372},{192,371},{192,370},{192,369},{192,368},{192,367},{192,366},{192,365},{192,364},{192,363},{192,362},{192,361},{192,360},{193,359},{193,358},{193,357},{193,356},{193,355},{193,354},{193,353},{193,352},{193,351},{193,350},{193,349},{193,348},{193,347},{193,346},{193,345},{193,344},{193,343},{194,342},{194,341},{194,340},{194,339},{194,338},{195,337},{195,336},{195,335},{195,334},{195,333},{195,332},{195,331},{195,330},{195,329},{195,328},{195,327},{195,326},{195,325},{195,324}},\
							/*edge28*/{{201,314},{200,313},{200,312},{199,311},{198,310},{198,309},{197,308},{196,307},{195,306},{194,305},{194,304},{194,303},{194,302},{194,301},{194,300},{194,299},{194,298},{194,297},{194,296},{194,295},{193,294},{193,293},{193,292},{193,291},{193,290},{194,289},{194,288},{194,287},{194,286},{194,285},{194,284},{194,283},{194,282},{195,281},{195,280},{195,279},{195,278},{195,277},{195,276},{195,275},{195,274},{196,273},{196,272},{196,271},{196,270},{196,269},{196,268},{197,267},{197,266},{197,265},{197,264},{197,263},{197,262},{198,261},{198,260},{198,259},{198,258},{198,257},{198,256},{199,255},{199,254},{199,253},{199,252},{199,251},{199,250},{199,249},{199,248},{199,247},{199,246},{199,245}},\
							/*edge29*/{{271,399},{271,400},{272,401},{271,401},{270,402},{269,403},{268,404},{267,404},{266,405},{266,406},{265,407},{264,408},{263,409},{262,410},{261,411},{260,412},{259,413},{258,414},{257,415},{256,416},{255,416},{254,417},{253,418},{252,418},{251,419},{250,419},{249,420},{248,420},{247,420},{246,421},{245,421},{244,421},{243,421},{242,421},{241,421},{240,421},{239,421},{238,421},{237,422},{236,422},{235,422},{234,422},{233,422},{232,422},{231,422},{230,422},{229,422},{228,423},{227,423},{226,423},{225,423},{224,423},{223,422},{222,422},{221,422},{220,422},{219,421},{218,421},{217,421},{216,420},{215,420},{214,419},{213,419},{212,418},{211,417},{210,416},{209,415},{208,414},{207,413},{206,412},{205,411},{204,411},{203,410},{202,409},{202,408},{201,407},{200,406},{200,405},{199,404},{199,403},{199,402},{198,401},{198,400},{198,399},{197,398},{197,397},{197,396},{197,395},{197,394},{197,393},{197,392},{197,391},{197,390},{197,389},{197,388},{197,387},{197,386},{197,385},{197,384},{197,383},{197,382},{197,381},{197,380},{198,379},{198,378},{198,377},{198,376},{198,375},{198,374},{199,373},{199,372},{199,371},{199,370},{199,369},{199,368},{200,367},{200,366},{200,365},{200,364},{200,363},{200,362},{200,361},{201,360},{201,359},{201,358},{201,357},{201,356},{201,355},{201,354},{201,353},{202,352},{202,351},{202,350},{202,349},{202,348},{202,347},{202,346},{202,345},{202,344},{203,343},{203,342},{203,341},{203,340},{203,339},{203,338},{204,337},{204,336},{204,335},{204,334},{204,333},{204,332},{204,331},{204,330},{203,329},{203,328},{202,327},{202,326},{201,325},{201,324}},\
							/*edge30*/{{315,272},{314,272},{313,273},{312,274},{311,275},{310,275},{309,276},{308,276},{307,277},{306,277},{305,277},{304,278},{303,278},{302,278},{301,279},{300,279},{299,279},{298,280},{297,280},{296,280},{295,280},{294,280},{293,280},{292,281},{291,281},{290,281},{289,281},{288,281},{287,281},{286,281},{285,282},{284,282},{283,282},{282,282},{281,282},{280,282},{279,282},{278,282},{277,282},{276,282},{275,282},{274,282},{273,282},{272,281},{271,281},{270,281},{269,281},{268,281},{267,280},{266,280},{265,280},{264,280},{263,280},{262,279},{261,279},{260,279},{259,279},{258,278},{257,278},{256,278},{255,277},{254,277},{253,277},{252,276},{251,276},{250,276},{249,275},{248,275},{247,275},{246,274},{245,274},{244,273},{243,273},{242,272},{241,272},{240,272},{239,271},{238,271},{237,271},{236,270},{235,270},{234,269},{233,269},{232,268},{231,267},{230,267},{229,266},{228,265},{227,265},{226,264},{225,263},{224,263},{223,262},{222,261},{221,261},{220,260},{219,259},{218,258},{217,257},{217,256},{216,255},{215,254},{214,253},{213,252},{213,251},{212,250},{211,249},{210,248},{209,247},{208,246},{208,245},{207,244},{207,243},{206,242},{206,241},{205,240},{205,239},{204,238}},\
							/*edge31*/{{210,398},{210,397},{210,396},{210,395},{211,394},{212,393},{213,392},{214,392},{215,391},{216,390},{217,389}},\
							/*edge32*/{{216,379},{216,378},{215,377},{215,376},{215,375},{215,374},{215,373},{215,372},{215,371},{215,370},{215,369},{216,368},{216,367},{216,366},{216,365},{216,364}},\
							/*edge33*/{{217,339},{217,338},{217,337},{217,336},{218,335},{218,334},{219,333},{219,332},{219,331},{219,330}},\
							/*edge34*/{{240,409},{239,410},{238,410},{237,410},{236,410},{235,410},{234,410},{233,410},{232,410},{231,410},{230,410},{229,410},{228,410},{227,410},{226,410},{225,410},{224,409},{223,409},{222,408},{221,407},{220,406},{219,405},{218,404}},\
							/*edge35*/{{235,347},{235,348},{235,349},{235,350},{235,351},{234,352},{234,353},{234,354},{234,355},{233,356},{233,357},{233,358},{233,359},{232,360}},\
							/*edge36*/{{276,336},{275,337},{275,338},{274,339},{273,340},{272,341},{271,342},{271,343},{271,344},{271,345},{271,346},{270,347},{270,348},{269,349},{268,349},{267,350},{266,350},{265,350},{264,351},{263,352},{263,353},{263,354},{263,355},{262,356},{262,357},{262,358},{262,359},{262,360},{262,361},{261,362},{261,363},{261,364},{261,365},{260,366},{260,367},{260,368},{260,369},{259,370},{259,371},{259,372},{259,373},{259,374},{258,375},{258,376},{258,377},{258,378},{258,379},{258,380},{257,381},{257,382},{256,383},{255,384},{254,385},{253,386},{252,387},{252,388},{251,389},{251,390},{250,391},{249,392},{248,393},{247,393},{246,394},{245,395},{245,396},{244,397},{243,398},{242,398}},\
							/*edge37*/{{253,426},{252,427},{251,427},{250,428},{249,429},{248,430},{248,431},{247,432},{247,433},{246,434}},\
							/*edge38*/{{250,374},{250,373},{250,372},{250,371},{251,370},{251,369},{251,368},{251,367},{251,366},{252,365},{252,364},{252,363},{253,362},{253,361},{253,360},{254,359},{254,358},{254,357},{255,356},{255,355},{255,354},{255,353},{256,352},{256,351}},\
							/*edge39*/{{255,438},{255,437},{255,436},{255,435},{255,434},{256,433},{256,432},{256,431},{257,430},{258,429}},\
							/*edge40*/{{269,333},{268,334},{267,335},{266,336},{265,336},{264,337},{263,337},{262,338},{261,338},{260,339},{259,339},{258,340}},\
							/*edge41*/{{272,314},{272,313},{272,312},{272,311},{271,310},{271,309},{271,308},{271,307},{271,306},{271,305},{271,304},{270,303},{270,302}},\
							/*edge42*/{{306,311},{306,312},{306,313},{305,314},{305,315},{305,316},{304,317},{304,318},{303,319},{303,320},{302,321},{302,322},{301,323},{301,324},{300,325},{300,326},{299,327},{299,328},{298,329},{298,330},{298,331},{298,332},{298,333},{297,334},{297,335},{297,336},{297,337},{296,338},{296,339},{295,340},{295,341},{295,342},{294,343},{294,344},{294,345},{293,346},{293,347},{292,348},{292,349},{291,350},{291,351},{291,352},{290,353},{290,354},{289,355},{289,356},{289,357},{288,358},{288,359},{288,360},{287,361},{287,362},{286,363},{286,364},{285,365},{284,366},{283,367},{282,368},{281,369},{280,370},{279,371},{278,372},{278,373},{277,374},{277,375},{277,376},{276,377},{276,378},{276,379},{276,380},{275,381},{275,382},{275,383},{274,384},{274,385},{274,386},{273,387},{273,388},{272,389}},\
							/*edge43*/{{293,289},{292,289},{291,289},{290,289},{289,289},{288,289},{287,289},{286,290},{285,290},{284,290},{283,291},{282,291},{281,291},{280,291},{279,292},{278,292},{277,292},{276,292},{275,293}},\
							/*edge44*/{{287,483},{288,484},{289,485},{289,486},{290,487},{291,488},{291,489},{292,490},{292,491},{292,492},{292,493},{292,494},{291,495},{290,496},{289,496},{288,496},{287,496},{286,495},{285,494},{284,493},{283,492},{282,491}},\
							/*edge45*/{{315,285},{315,286},{315,287},{315,288},{314,289},{314,290},{313,291},{313,292},{313,293},{313,294},{312,295},{312,296},{312,297},{311,298},{311,299},{310,300},{309,301},{308,302},{307,303},{307,304},{306,305},{306,306},{306,307},{306,308},{305,309}},\
							/*edge46*/{{337,373},{336,373},{335,372},{334,372},{333,372},{332,371},{331,371},{330,371},{329,371},{328,371},{327,370},{326,370},{325,369},{324,369},{323,368},{322,367},{321,366},{320,365},{320,364},{320,363},{319,362},{318,361},{317,361},{316,361},{315,360},{314,360},{313,359},{312,358},{311,357},{310,356},{309,355},{309,354},{309,353},{309,352},{309,351},{309,350},{309,349},{310,348},{310,347},{311,346},{312,345},{312,344},{313,343},{314,342},{315,342},{316,341},{317,340},{318,340},{319,339},{320,338},{321,337},{321,336},{322,335},{323,334},{323,333},{324,332},{325,331},{325,330},{326,329},{326,328},{327,327},{327,326},{328,325},{328,324},{328,323},{329,322},{329,321},{329,320},{329,319},{328,318},{328,317},{328,316},{327,315},{327,314},{327,313},{327,312},{327,311},{327,310},{327,309}},\
							/*edge47*/{{333,254},{332,255},{332,256},{331,257},{330,258},{329,259},{329,260},{328,261},{327,262},{326,263}},\
							/*edge48*/{{346,339},{345,339},{344,340},{343,340},{342,341},{341,341},{340,341},{339,342},{338,342},{337,342},{336,343},{335,344},{335,345},{334,346},{333,346},{332,347},{331,347},{330,347},{329,347},{328,347}},\
							/*edge49*/{{344,294},{343,295},{342,295},{341,296},{340,297},{339,298},{338,299},{337,300},{336,301},{335,301},{334,301}},\
							/*edge50*/{{339,314},{339,315},{338,316},{337,317},{336,318},{336,319},{336,320},{336,321},{335,322},{335,323},{335,324},{335,325},{334,326}},\
							/*edge51*/{{387,403},{386,403},{385,403},{384,403},{383,403},{382,403},{381,403},{380,402},{379,402},{378,401},{377,401},{376,401},{375,400},{374,399},{373,398},{372,397},{371,396},{370,396},{369,395},{368,394},{367,394},{366,394},{365,394},{364,394},{363,393},{362,393},{361,393},{360,392},{359,392},{358,392},{357,391},{356,391},{355,390},{354,390},{353,390},{352,390},{351,389},{350,389},{349,389},{348,388},{347,388},{346,387},{345,387},{344,387},{343,386},{342,385},{341,384},{341,383},{341,382},{341,381}},\
							/*edge52*/{{355,182},{355,181},{356,180},{356,179},{357,178},{357,177},{358,176},{358,175},{359,174},{359,173},{359,172},{359,171},{360,170},{360,169},{361,168}},\
							/*edge53*/{{419,292},{418,293},{417,293},{416,293},{415,293},{414,293},{413,293},{412,293},{411,293},{410,292},{409,292},{408,292},{407,292},{406,292},{405,292},{404,292},{403,292},{402,293},{401,294},{400,295},{400,296},{399,297},{398,298},{397,299},{396,299},{395,300},{394,301},{393,301},{392,302},{391,303},{391,304},{390,305},{389,306},{389,307},{388,308},{387,309},{386,309},{385,310},{384,310},{383,310},{382,310}},\
							/*edge54*/{{396,200},{395,201},{394,201},{393,201},{392,201},{391,200},{390,199},{389,198},{389,197},{389,196},{390,195},{391,194},{392,194}},\
							/*edge55*/{{412,236},{412,237},{411,238},{410,239},{410,240},{409,241},{408,242},{407,242},{406,243},{405,244},{404,244},{403,244},{402,244},{401,244},{400,244},{399,245},{398,245},{397,245},{396,245},{395,245}}};


