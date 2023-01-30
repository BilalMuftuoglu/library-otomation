#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ogrenci{
    char ogrID[9];
    char ad[30];
    char soyad[30];
    int puan;
    struct Ogrenci* next;
    struct Ogrenci* prev;
}OGRENCI; 

typedef struct Yazar{
    int yazarID;
    char yazarAd[30];
    char yazarSoyad[30];
    struct Yazar*next;
}YAZAR;

typedef struct KitapOrnek{
     char EtiketNo[20];
     char Durum[9];
     struct KitapOrnek *next;
}KITAPORNEK;

typedef struct Kitap{
    char kitapAdı[30];
    char ISBN[14];
    int adet;
    struct Kitap *next;
    KITAPORNEK *head;
}KITAP;

typedef struct KitapYazar{
    char ISBN[14];
    int YazarID;
    struct KitapYazar *next;
}KITAPYAZAR;

typedef struct Tarih{
    unsigned int gun:5;
    unsigned int ay:4;
    unsigned int yil:12;
}TARIH;

typedef struct KitapOdunc{
    char EtiketNo[20];
    char ogrID[9];
    int islemTipi;
    TARIH islemTarihi;
    struct KitapOdunc *next;
    struct KitapOdunc *prev;
}KITAPODUNC;

int kitapOduncOgrenciKarsilastir(KITAPODUNC* current,char no[]){
    //Bir öğrenci silindiğinden dolayı kitap-ödünç kaydı silinirken kullanılan fonksiyon pointer
    if(strcmp(no, current->ogrID) == 0){
        return 1;
    }else{
        return 0;
    }
}

int kitapOduncKitapKarsilastir(KITAPODUNC* current,char isbn[]){
    //Bir kitap silindiğinden dolayı kitap-ödünç kaydı silinirken kullanılan fonksiyon pointer
    if(strstr(current->EtiketNo,isbn)!=NULL){
        return 1;
    }else{
        return 0;
    }
}

int kitapAdKarsilastir(KITAP *current, char ad[]){
    //Kitap adı ile kitap arama fonksiyon pointer
    if(strcmp(ad, current->kitapAdı) == 0){
        return 1;
    }else{
        return 0;
    }
}

int kitapIsbnKarsilastir(KITAP *current, char isbn[]){
    //Kitap ISBN ile kitap arama fonksiyon pointer
    if(strcmp(isbn, current->ISBN) == 0){
        return 1;
    }else{
        return 0;
    }
}

void kitap_free(KITAP** kitapHead){
    //Kitap linked listesini silen fonksiyon
    KITAP *temp = *kitapHead;
    while(*kitapHead != NULL){
        *kitapHead = (*kitapHead)->next;
        free(temp);
        temp = *kitapHead;
    }
}

void kitap_odunc_free(KITAPODUNC** oduncHead){
    //KitapOdunc linked listesini silen fonksiyon
    KITAPODUNC *temp = *oduncHead;
    while(*oduncHead != NULL){
        *oduncHead = (*oduncHead)->next;
        free(temp);
        temp = *oduncHead;
    }
}

void kitap_odunc_ekle(KITAPODUNC** oduncHead,KITAPODUNC** oduncTail,char etiket[],char ogrId[],int islem,TARIH* islemTarihi){
    //KitapOdunc linked listesine ekleme yapan fonksiyon
    KITAPODUNC *temp = (KITAPODUNC*)malloc(sizeof(KITAPODUNC));
    if(temp==NULL){
        printf("\nKitap odunc alma basarisiz!!!\n\n");
        return;
    }
    strcpy(temp->EtiketNo,etiket);
    strcpy(temp->ogrID,ogrId);
    temp->islemTipi=islem;
    temp->islemTarihi=*islemTarihi;
    temp->next=NULL;
    temp->prev=NULL;
    if(*oduncHead==NULL){
        *oduncHead=temp;
        *oduncTail=temp;
    }else{
        (*oduncTail)->next=temp;
        temp->prev=*oduncTail;
        *oduncTail=temp;
    }
}

void kitapOdunc_dosyadan_oku(KITAPODUNC** oduncHead,KITAPODUNC** oduncTail){
    //Programın başında csv dosyasından okuma yapan fonksiyon
    FILE *fp = fopen("KitapOdunc.csv", "r");
    if (fp == NULL) {
        printf("\nKitapOdunc dosyasi acilamadi!!!\n");
        return;
    }
    TARIH tarih;
    char line[100];
    char *token;
    char* etiketNo;
    char *ogrNo;
    int sonuc;
    int islemTipi;
    while(fgets(line, 100, fp) != NULL){
        token = strtok(line, ",");
        etiketNo = token;
        token = strtok(NULL, ",");
        ogrNo = token;
        token = strtok(NULL, ",");
        islemTipi = atoi(token);
        token = strtok(NULL, ".");
        tarih.gun = atoi(token);
        token = strtok(NULL, ".");
        tarih.ay = atoi(token);
        token = strtok(NULL, "\n");
        tarih.yil = atoi(token);
        kitap_odunc_ekle(oduncHead,oduncTail, etiketNo,ogrNo,islemTipi,&tarih);
    }
    fclose(fp);
}

void kitapOdunc_dosyaya_yaz(KITAPODUNC *oduncHead){
    //Program kapanırken csv dosyasına yazma işlemini yapan fonksiyon
    FILE *fp = fopen("KitapOdunc.csv", "w");
    if (fp == NULL) {
        printf("\nKitapOdunc dosyasi acilamadi!!!\n");
        return;
    }
    KITAPODUNC *iter = oduncHead;
	while(iter != NULL){
		fprintf(fp, "%s,%s,%d,%u.%u.%u\n",iter->EtiketNo,iter->ogrID,iter->islemTipi,iter->islemTarihi.gun,iter->islemTarihi.ay,iter->islemTarihi.yil);
		iter = iter->next;
	}
	fclose(fp);	
}

void kitap_odunc_sil(KITAPODUNC** oduncHead,KITAPODUNC** oduncTail,int type,char silinecek[]){
    //Eğer bir öğrenci veya kitap silinirse onlarla alakalı tüm kitap ödünç kayıtları silinir(kitap type = 0, öğrenci type = 1)
    KITAPODUNC *temp = *oduncHead;
    int (*fptr[2])(KITAPODUNC*,char[])= {kitapOduncKitapKarsilastir, kitapOduncOgrenciKarsilastir};
    while(temp != NULL){
        if(fptr[type](temp,silinecek)){
            if(temp->prev == NULL){
                *oduncHead = temp->next;
                if(temp->next != NULL){
                    temp->next->prev = NULL;
                }
            }else if(temp->next == NULL){
                *oduncTail = temp->prev;
                if(temp->prev != NULL){
                    temp->prev->next = NULL;
                }
            }else{
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            free(temp);
        }
        temp = temp->next;
    }
}

void kitap_odunc_al(KITAP **kitapHead,KITAPODUNC** oduncHead,KITAPODUNC** oduncTail,OGRENCI* ogrenciHead,int (*fptr)(KITAP*,char[])){
    //Öğrenci kitap ödünç alırken kullanılan fonksiyon
    char ogrID[9];
    char ISBN[14];
    char EtiketNo[20];
    char arananKitap[20];
    unsigned int tmp;
    printf("Aramak istediginiz kitap bilgisini giriniz: ");
    scanf(" %[^\n]s",arananKitap);
    KITAP *kitap = *kitapHead;
    while(kitap != NULL && !fptr(kitap,arananKitap)){
        kitap = kitap->next;
    }
    if(kitap == NULL){
        printf("\nKitap bulunamadi!!!\n\n");
        return;
    }
    
    KITAPORNEK *kitapOrnek = kitap->head;
    while(kitapOrnek != NULL && strcmp(kitapOrnek->Durum,"RAFTA")){
        kitapOrnek = kitapOrnek->next;
    }
    if(kitapOrnek == NULL){
        printf("\nBosda kitap ornegi bulunamadi!!!\n\n");
        return;
    }

    printf("Ogrenci ID: ");
    scanf("%s",ogrID);
    OGRENCI *ogrenci = ogrenciHead;
    while(ogrenci != NULL && strcmp(ogrenci->ogrID,ogrID) != 0){
        ogrenci = ogrenci->next;
    }
    if(ogrenci == NULL){
        printf("\nOgrenci bulunamadi!!!\n\n");
        return;
    }
    if(ogrenci->puan <= 0){
        printf("\nOgrenci, puani yetersiz oldugundan dolayi kitap odunc alamaz!!!\n\n");
        return;
    }

    KITAPORNEK *kitapOrnekKontrol = kitap->head;
    while(kitapOrnekKontrol != NULL && strcmp(kitapOrnekKontrol->Durum,ogrID)){
        kitapOrnekKontrol = kitapOrnekKontrol->next;
    }
    if(kitapOrnekKontrol != NULL){
        printf("\nOgrenci zaten ayni kitaptan odunc almis!!!\n\n");
        return;
    }
    
    KITAPODUNC *kitapOdunc = (KITAPODUNC *)malloc(sizeof(KITAPODUNC));
    printf("Islem Tarihi Gün: ");
    scanf("%u",&tmp);
    kitapOdunc->islemTarihi.gun=tmp;
    printf("Islem Tarihi Ay: ");
    scanf("%u",&tmp);
    kitapOdunc->islemTarihi.ay=tmp;
    printf("Islem Tarihi Yil: ");
    scanf("%u",&tmp);
    kitapOdunc->islemTarihi.yil=tmp;
    strcpy(kitapOdunc->ogrID,ogrID);
    strcpy(kitapOdunc->EtiketNo,kitapOrnek->EtiketNo);
    kitapOdunc->islemTipi = 0;
    strcpy(kitapOrnek->Durum,ogrID);
    kitapOdunc->next = NULL;
    kitap_odunc_ekle(oduncHead,oduncTail,kitapOdunc->EtiketNo,kitapOdunc->ogrID,kitapOdunc->islemTipi,&kitapOdunc->islemTarihi);
}

OGRENCI* ogrenci_bul(OGRENCI* ilk_ogrenci,char id[]){
    //ID'ye göre öğrenci dönen fonksiyon
    OGRENCI *ogrenci = ilk_ogrenci;
    while(ogrenci != NULL){
        if(strcmp(ogrenci->ogrID,id)==0){
            return ogrenci;
        }
        ogrenci = ogrenci->next;
    }
    return NULL;
}

void zamaninda_teslim_edilmeyen_kitaplar_listele(KITAPODUNC* kitapOduncHead,KITAP* kitapHead,OGRENCI* ogrenciHead){
    //Zamanında teslim edilmeyen kitapları listeleme fonksiyonu
    KITAPODUNC* kitapOdunc = kitapOduncHead;
    KITAPODUNC* tmp;
    printf("\n***************ZAMANINDA TESLIM EDILMEYEN KITAPLAR***************\n");
    printf("\nKitap Adi - ISBN - Adet - Etiket No - Durum - Ogrenci Adi - Ogrenci Soyadi - Ogrenci Puani - Ogrenci no - Odunc Alma Tarihi - Teslim Etme Tarihi\n\n");
    while(kitapOdunc != NULL){
        if(kitapOdunc->islemTipi ==0){
            tmp = kitapOdunc->next;
            while(tmp != NULL && strcmp(kitapOdunc->EtiketNo,tmp->EtiketNo) && strcmp(kitapOdunc->ogrID,tmp->ogrID)){
                tmp = tmp->next;
            }
            if(tmp!=NULL){
                int yilFarki = tmp->islemTarihi.yil - kitapOdunc->islemTarihi.yil;
                int ayFarki = tmp->islemTarihi.ay - kitapOdunc->islemTarihi.ay;
                int gunFarki = tmp->islemTarihi.gun - kitapOdunc->islemTarihi.gun;
                int toplamFark = yilFarki*365 + ayFarki*30 + gunFarki;
                if(toplamFark > 15){
                    KITAP* kitap = kitapHead;
                    while(kitap != NULL && strstr(tmp->EtiketNo,kitap->ISBN)==NULL){
                        kitap = kitap->next;
                    }
                    if(kitap==NULL){
                        printf("Hata!!!\n");
                        return;
                    }
                    KITAPORNEK* kitapOrnek = kitap->head;
                    while(kitapOrnek != NULL && strcmp(kitapOrnek->EtiketNo,tmp->EtiketNo)){
                        kitapOrnek = kitapOrnek->next;
                    }
                    if(kitapOrnek==NULL){
                        printf("Hata2!!!\n");
                        return;
                    }
                    OGRENCI *ogrenci = ogrenci_bul(ogrenciHead,tmp->ogrID);
                    printf("%s , %s , %d , %s , %s , %s , %s , %d , %s , %d/%d/%d - %d/%d/%d\n",kitap->kitapAdı,kitap->ISBN,kitap->adet,kitapOrnek->EtiketNo,kitapOrnek->Durum,ogrenci->ad,ogrenci->soyad,ogrenci->puan,ogrenci->ogrID,kitapOdunc->islemTarihi.gun,kitapOdunc->islemTarihi.ay,kitapOdunc->islemTarihi.yil,tmp->islemTarihi.gun,tmp->islemTarihi.ay,tmp->islemTarihi.yil);
                }
            }
        }
        kitapOdunc = kitapOdunc->next;
    }
    printf("\n*****************************************************************\n");
}

void kitap_teslim_et(KITAP **kitapHead,KITAPODUNC** oduncHead,KITAPODUNC** oduncTail, OGRENCI**ogrenciHead,int (*fptr)(KITAP*,char[])){
    //Kitap teslim etme fonksiyonu
    char ogrID[9];
    char ISBN[14];
    char EtiketNo[20];
    char arananKitap[20];
    unsigned int tmp;
    printf("Aramak istediginiz kitap bilgisini giriniz: ");
    scanf(" %[^\n]s",arananKitap);
    KITAP *kitap = *kitapHead;
    while(kitap != NULL && !fptr(kitap,arananKitap)){
        kitap = kitap->next;
    }
    if(kitap == NULL){
        printf("\nKitap bulunamadi!!!\n\n");
        return;
    }

    printf("Ogrenci ID: ");
    scanf("%s",ogrID);
    OGRENCI *ogrenci = *ogrenciHead;
    while(ogrenci != NULL && strcmp(ogrenci->ogrID,ogrID) != 0){
        ogrenci = ogrenci->next;
    }
    if(ogrenci == NULL){
        printf("\nOgrenci bulunamadi!!!\n\n");
        return;
    }
    
    KITAPORNEK *kitapOrnek = kitap->head;
    while(kitapOrnek != NULL && strcmp(kitapOrnek->Durum,ogrenci->ogrID)){
        kitapOrnek = kitapOrnek->next;
    }
    if(kitapOrnek == NULL){
        printf("\nOgrenciye ait kitap ornegi bulunamadi!!!\n\n");
        return;
    }

    KITAPODUNC *kitapOduncAl = *oduncTail;
    while(kitapOduncAl != NULL && strcmp(kitapOduncAl->ogrID,ogrID) && strcmp(kitapOduncAl->EtiketNo,kitapOrnek->EtiketNo)){
        kitapOduncAl = kitapOduncAl->prev;
    }
    if(kitapOduncAl == NULL){
        printf("\nOgrenciye ait teslim alinan kayit bulunamadi!!!\n\n");
        return;
    }

    KITAPODUNC *kitapOdunc = (KITAPODUNC *)malloc(sizeof(KITAPODUNC));
    printf("Islem Tarihi Gün: ");
    scanf("%u",&tmp);
    kitapOdunc->islemTarihi.gun=tmp;
    printf("Islem Tarihi Ay: ");
    scanf("%u",&tmp);
    kitapOdunc->islemTarihi.ay=tmp;
    printf("Islem Tarihi Yil: ");
    scanf("%u",&tmp);
    kitapOdunc->islemTarihi.yil=tmp;

    int yilFarki = kitapOdunc->islemTarihi.yil - kitapOduncAl->islemTarihi.yil;
    int ayFarki = kitapOdunc->islemTarihi.ay - kitapOduncAl->islemTarihi.ay;
    int gunFarki = kitapOdunc->islemTarihi.gun - kitapOduncAl->islemTarihi.gun;
    int toplamFark = yilFarki*365 + ayFarki*30 + gunFarki;
    if(toplamFark > 15){
        printf("\n15 gunden sonra teslim edildigi icin puan kesintisi uygulanmistir!!!\n");
        ogrenci->puan -= 10;
    }
    strcpy(kitapOdunc->ogrID,ogrID);
    strcpy(kitapOdunc->EtiketNo,kitapOrnek->EtiketNo);
    kitapOdunc->islemTipi = 1;
    strcpy(kitapOrnek->Durum,"RAFTA");
    kitapOdunc->next = NULL;
    kitap_odunc_ekle(oduncHead,oduncTail,kitapOdunc->EtiketNo,kitapOdunc->ogrID,kitapOdunc->islemTipi,&kitapOdunc->islemTarihi);
}

char* kitapOrnek_kontrol(KITAPODUNC** oduncTail,char etiket[]){
    //Kitap ekleme fonksiyonunda eğer kitap bilgileri csv'den okunuyorsa 
    //csv dosyasında kitap durum bilgileri olmadığından dolayı 
    //kitap örneğinin durum bilgisini kitap ödünç linked listesinden kontrol eden fonksiyon
    KITAPODUNC *kitapOdunc = *oduncTail;
    while(kitapOdunc != NULL && strcmp(kitapOdunc->EtiketNo,etiket)){
        kitapOdunc = kitapOdunc->prev;
    }
    if(kitapOdunc == NULL){
        return NULL;
    }
    if(kitapOdunc->islemTipi==0){
        return kitapOdunc->ogrID;
    }else{
        return NULL;
    }
}

void kitap_ekle(KITAP **head,char kitapAdi[],char isbn[],int adet,int eklemeTipi,KITAPODUNC** oduncTail){
    //Kitap ekleme fonksiyonu (kullanıcı kitap ekliyorsa ekleme tipi = 0, csv'den okunuyorsa ekleme tipi = 1)
    KITAP *kitap = *head;
    while(kitap != NULL && strcmp(kitap->ISBN,isbn) != 0){
        kitap = kitap->next;
    }
    if(kitap != NULL){
        printf("\nAyni ISBN numarali kitap bulunmaktadir!!!\n\n");
        return;
    }

    KITAP *tmp = (KITAP *)malloc(sizeof(KITAP));
    if(tmp==NULL){
        printf("\nKitap ekleme islemi basarisiz!!!\n");
        return;
    }
    strcpy(tmp->kitapAdı,kitapAdi);
    strcpy(tmp->ISBN,isbn);
    tmp->adet=adet;
    tmp->next = NULL;
    tmp->head = NULL;
    char* id;
    KITAPORNEK *iter = NULL;
    for(int i=1; i<=tmp->adet; i++){
        KITAPORNEK *ornek = (KITAPORNEK *)malloc(sizeof(KITAPORNEK));
        sprintf(ornek->EtiketNo,"%s_%d",tmp->ISBN,i);
        if(eklemeTipi==0){
            strcpy(ornek->Durum,"RAFTA");
        }else if(eklemeTipi==1){
            id = kitapOrnek_kontrol(oduncTail,ornek->EtiketNo);
            if(id==NULL){
                strcpy(ornek->Durum,"RAFTA");
            }else{
                strcpy(ornek->Durum,id);
            }
        }
        
        ornek->next = NULL;
        if(tmp->head == NULL){
            tmp->head = ornek;
        }
        else{
            iter->next = ornek;
        }
        iter = ornek;
    }

    if(*head == NULL){
        *head = tmp;
    }
    else{
        KITAP *gecici = *head;
        while(gecici->next != NULL){
            gecici = gecici->next;
        }
        gecici->next = tmp;
    }
}

void kitap_listele(KITAP *head){
    //Tüm kitapların bilgilerini listeleyen fonksiyon
    if(head==NULL){
        printf("\nKayitli kitap yok!!!\n\n");
        return;
    }
    KITAP *iter=head;
    KITAPORNEK *ornek;
    printf("\n***************KITAPLAR***************\n");
    printf("\nKitap Adi - ISBN - Adet - Etiket No - Durum\n\n");
    while(iter!=NULL){
        ornek=iter->head;
        while(ornek!=NULL){
            if(!strcmp(ornek->Durum,"RAFTA")){
                printf("%s , %s , %d , %s , %s\n", iter->kitapAdı, iter->ISBN, iter->adet, ornek->EtiketNo, ornek->Durum);
            }
            ornek=ornek->next;
        }
        iter=iter->next;
    }
    printf("\n****************************************\n");
}

void kitap_bilgi(KITAP *head,int (*fptr)(KITAP*,char[]),YAZAR* yazarHead,KITAPYAZAR* kitapYazarHead){
    //Tek bir kitap ve kitabın yazarlarının bilgilerini gösteren fonksiyon
    KITAP *current = head;
    char aranan[20];
    printf("Aramak istediginiz kitap bilgisini giriniz: ");
    scanf(" %[^\n]s",aranan);
    while(current != NULL && !fptr(current,aranan)){
        current = current->next;
    }
    
    KITAPORNEK *ornek;
    if(current == NULL){
        printf("\nKitap bulunamadi!!!\n\n");
    }else{
        ornek=current->head;
        printf("\n***************KITAPLAR***************\n");
        printf("\nKitap Adi - ISBN - Adet - Etiket No - Durum\n\n");
        while(ornek!=NULL){
            printf("%s , %s , %d , %s , %s\n", current->kitapAdı, current->ISBN, current->adet, ornek->EtiketNo, ornek->Durum);
            ornek=ornek->next;
        }
        printf("\n***************KITABIN YAZARLARI***************\n");
        printf("\nYazar ID - Yazar Adi - Yazar Soyadi\n\n");
        KITAPYAZAR* kitapYazar = kitapYazarHead;
        while(kitapYazar!=NULL){
            if(!strcmp(kitapYazar->ISBN,current->ISBN)){
                YAZAR* yazar = yazarHead;
                while(yazar!=NULL){
                    if(yazar->yazarID == kitapYazar->YazarID){
                        printf("%d , %s , %s\n",yazar->yazarID,yazar->yazarAd,yazar->yazarSoyad);
                    }
                    yazar=yazar->next;
                }
            }
            kitapYazar=kitapYazar->next;
        }
        printf("\n****************************************\n");
    }
}

void kitapYazar_free(KITAPYAZAR** kitapYazarHead){
    //Kitap-yazar eşleştirmelerini tutan linked listi silen fonksiyon
    KITAPYAZAR *temp = *kitapYazarHead;
    while(*kitapYazarHead != NULL){
        *kitapYazarHead = (*kitapYazarHead)->next;
        free(temp);
        temp = *kitapYazarHead;
    }
}

void kitapYazar_sil(KITAPYAZAR** kitapYazarHead,char silinenKitap[]){
    //Bir kitap silindiğinde o kitabın tüm kitap-yazar eşleştirmelerini silen fonksiyon
    if(*kitapYazarHead==NULL){
        return;
    }
    KITAPYAZAR *current = *kitapYazarHead;
    KITAPYAZAR *tmp = current;
    KITAPYAZAR *prev = NULL;
    while(current != NULL){
        if(!strcmp(current->ISBN,silinenKitap)){
            if(prev == NULL){
                *kitapYazarHead = current->next;
            }else{
                prev->next = current->next;
            }
            tmp = current;
            current = current->next;
            free(tmp);
        }else{
            prev = current;
            current = current->next;
        }
    }
}

void kitap_sil(KITAP **head,int (*fptr)(KITAP*,char[]),KITAPODUNC **oduncHead, KITAPODUNC **oduncTail,KITAPYAZAR** kitapYazarHead){
    //Kitap silme fonksiyonu
    KITAP* current = *head;
    KITAP* prev = NULL;
    char aranan[20];
    printf("Silmek istediginiz kitap bilgisini giriniz: ");
    scanf(" %[^\n]s",aranan);

    if (fptr(current,aranan)){
        kitap_odunc_sil(oduncHead,oduncTail,0,current->ISBN);//kitap silindiğinde ilgili tüm ödünç işlemleri silinir
        kitapYazar_sil(kitapYazarHead,current->ISBN);//kitap silindiğinde ilgili tüm yazar eşleştirmeleri silinir
        *head = current->next;
        free(current);
        return;
    }

    while (current != NULL && !fptr(current,aranan)){
        prev = current;
        current = current->next;
    }
    if(current==NULL){
        printf("\nKitap bulunamadi!!!\n\n");
        return;
    }
    kitap_odunc_sil(oduncHead,oduncTail,0,current->ISBN);//kitap silindiğinde ilgili tüm ödünç işlemleri silinir
    kitapYazar_sil(kitapYazarHead,current->ISBN);//kitap silindiğinde ilgili tüm yazar eşleştirmeleri silinir
    prev->next = current->next;
    free(current);
}

void kitap_guncelle(KITAP **head,int (*fptr)(KITAP*,char[])){
    //Kitap güncelleme fonksiyonu
    KITAP* current = *head;
    char aranan[20];
    printf("Guncellemek istediginiz kitap bilgisini giriniz: ");
    scanf(" %[^\n]s",aranan);
	while(current != NULL && !fptr(current,aranan)){
		current = current->next;
	}
	if(current != NULL){
        printf("Yeni kitap adi giriniz: ");
        scanf(" %[^\n]s",current->kitapAdı);
	}else{
        printf("\nKitap bulunamadi!!!\n\n");
    }
}

void kitap_dosyaya_yaz(KITAP* head){
    FILE *fp = fopen("Kitaplar.csv", "w");
    if (fp == NULL) {
        printf("\nKitap dosyasi acilamadi!!!\n");
        return;
    }
	KITAP *iter = head;
	while(iter != NULL){
		fprintf(fp, "%s,%s,%d\n", iter->kitapAdı, iter->ISBN, iter->adet);
		iter = iter->next;
	}
	fclose(fp);
}

void kitap_dosyadan_oku(KITAP** head,KITAPODUNC** oduncTail){
    FILE *fp = fopen("Kitaplar.csv", "r");
    if (fp == NULL) {
        printf("\nKitap dosyasi acilamadi!!!\n");
        return;
    }
    char line[100];
    char *token;
    char* kitapAdi;
    char *ISBN;
    int adet;
    while(fgets(line, 100, fp) != NULL){
        token = strtok(line, ",");
        kitapAdi =  token;
        token = strtok(NULL, ",");
        ISBN = token;
        token = strtok(NULL, "\n");
        adet = atoi(token);
        kitap_ekle(head, kitapAdi, ISBN, adet,1,oduncTail);
    }
    fclose(fp);
}

void yazar_free(YAZAR** yazarHead){
    //Yazar listesini silen fonksiyon
    YAZAR *temp = *yazarHead;
    while(*yazarHead != NULL){
        *yazarHead = (*yazarHead)->next;
        free(temp);
        temp = *yazarHead;
    }
}

void kitapYazar_listele(KITAPYAZAR* kitapYazarHead){
    //Kitap-yazar eşleştirmelerini listeleme fonksiyonu
    if(kitapYazarHead==NULL){
        printf("\nKayitli kitap-yazar eslestirmesi yok!!!\n\n");
        return;
    }
	KITAPYAZAR *current = kitapYazarHead;
    printf("\n***************KITAP-YAZAR***************\n");
    printf("\nKitap ISBN  -  Yazar ID\n\n");
	while(current != NULL){
        printf("%s , %d\n",current->ISBN,current->YazarID);
        current = current->next;
	}
    printf("\n****************************************\n");
}

void kitapYazar_yazar_guncelle(KITAPYAZAR** kitapYazarHead,int silinenYazarId){
    //Yazar silinirse o yazara ait eşleştirmeler -1 olarak güncellenir
    KITAPYAZAR *current = *kitapYazarHead;
    while(current != NULL){
        if(current->YazarID==silinenYazarId){
            current->YazarID=-1;
        }
        current = current->next;
    }
}

void kitapYazar_guncelle(KITAPYAZAR** kitapYazarHead,YAZAR* yazarHead){
    //Kitap-yazar eşleştirmelerini güncelleme fonksiyonu
    char arananKitap[14];
    printf("Yazar guncelleme islemi yapilacak kitap ISBN giriniz: ");
    scanf("%s",arananKitap);
    KITAPYAZAR *current = *kitapYazarHead;
    int ID = 0;
    while(current != NULL){
        if(!strcmp(arananKitap,current->ISBN)){
            printf("Kitap yazarinin ID'si: %d -- Yeni ID giriniz: ",current->YazarID);
            scanf("%d",&ID);
            while(1){
                YAZAR *yazar = yazarHead;
                while(yazar != NULL && yazar->yazarID != ID){
                    yazar = yazar->next;
                }
                if(yazar == NULL){
                    printf("Bu ID'ye sahip bir yazar bulunamadi. Lutfen gecerli bir ID giriniz: ");
                    scanf("%d",&ID);
                }else{
                    current->YazarID=ID;
                    break;
                }
            }
        }
        current = current->next;
    }
    if (ID==0){
        printf("\nKitap bulunamadi!!!\n\n");
    }
}

void kitapYazar_ekle(KITAP* kitapHead,YAZAR* yazarHead,KITAPYAZAR** kitapYazarHead,int (*fptr)(KITAP*,char[]),char arananKitap[], int yazarId){
    //Kitap-yazar eşleştirmelerini ekleme fonksiyonu
    KITAPYAZAR *current = *kitapYazarHead;
    while(current != NULL){
        if(!strcmp(arananKitap,current->ISBN) && current->YazarID==yazarId){
            printf("\nBu kitap ve yazar zaten eslestirilmis!!!\n\n");
            return;
        }
        current = current->next;
    }

    KITAPYAZAR *temp = (KITAPYAZAR*)malloc(sizeof(KITAPYAZAR));
    if(temp==NULL){
        printf("\nKitap yazar ekleme basarisiz\n");
        return;
    }
    if(kitapHead==NULL){
        printf("\nKayitli kitap yok!!!\n\n");
        return;
    }
    if(yazarId!=-1 && yazarHead==NULL){
        printf("\nKayitli yazar yok!!!\n\n");
        return;
    }else if(yazarId!=-1){
        YAZAR *yazar = yazarHead;
        while(yazar != NULL && yazar->yazarID != yazarId){
            yazar = yazar->next;
        }
        if(yazar == NULL){
            printf("\nYazar bulunamadi!!!\n\n");
            return;
        }
        temp->YazarID = yazar->yazarID;
    }else{
        temp->YazarID = -1;
    }
    KITAP *kitap = kitapHead;
    while(kitap != NULL && !fptr(kitap,arananKitap)){
        kitap = kitap->next;
    }
    if(kitap == NULL){
        printf("\nKitap bulunamadi!!!\n\n");
        return;
    }
    
    strcpy(temp->ISBN,kitap->ISBN);
    temp->next = NULL;
    if(*kitapYazarHead == NULL){
        *kitapYazarHead = temp;
    }
    else{
        KITAPYAZAR *iter = *kitapYazarHead;
        while(iter->next != NULL){
            iter = iter->next;
        }
        iter->next = temp;
    }
}

void kitapYazar_dosyaya_yaz(KITAPYAZAR* head){
    FILE *fp = fopen("KitapYazar.csv", "w");
    if (fp == NULL) {
        printf("\nKitapYazar dosyasi acilamadi!!!\n");
        return;
    }
    KITAPYAZAR *iter = head;
    while(iter != NULL){
        fprintf(fp, "%s,%d\n", iter->ISBN, iter->YazarID);
        iter = iter->next;
    }
    fclose(fp);
}

void kitapYazar_dosyadan_oku(KITAPYAZAR** head,KITAP* kitapHead,YAZAR* yazarHead){
    FILE *fp = fopen("KitapYazar.csv", "r");
    if (fp == NULL) {
        printf("\nKitapYazar dosyasi acilamadi!!!\n");
        return;
    }
    char line[100];
    char *token;
    char* ISBN;
    int yazarID;
    while(fgets(line, 100, fp) != NULL){
        token = strtok(line, ",");
        ISBN =  token;
        token = strtok(NULL, "\n");
        yazarID = atoi(token);
        kitapYazar_ekle(kitapHead,yazarHead,head,kitapIsbnKarsilastir,ISBN,yazarID);
    }
    fclose(fp);
}

void yazar_ekle(YAZAR **head, char ad[], char soyad[],int yazarID){
	YAZAR *temp = (YAZAR*)malloc(sizeof(YAZAR));
    if(temp==NULL){
        printf("\nYazar ekleme basarisiz!!!\n");
        return;
    }
	strcpy(temp->yazarAd, ad);
	strcpy(temp->yazarSoyad, soyad);
    temp->yazarID = yazarID;
	temp->next = NULL;
    int id;
	if(*head == NULL){
        if(yazarID==0){
            temp->yazarID = 1;
        }else{
            temp->yazarID = yazarID;
        }
		*head = temp;
	}
	else{
		YAZAR *iter = *head;
		while(iter->next != NULL){
			iter = iter->next;
            id=iter->yazarID;
		}
        if(yazarID==0){
            temp->yazarID = id+1;
        }else{
            temp->yazarID = yazarID;
        }
		iter->next = temp;
	}
}

void yazar_listele(YAZAR *head){
    //Tüm yazarların bilgilerini listeleyen fonksiyon
    if(head==NULL){
        printf("\nKayitli yazar yok!!!\n\n");
        return;
    }
	YAZAR *iter = head;
    printf("\n***************YAZARLAR***************\n");
    printf("\nYazar Id  -  Yazar Adi  -  Yazar Soyadi\n\n");
	while(iter != NULL){
        printf("%d , %s , %s\n", iter->yazarID,iter->yazarAd,iter->yazarSoyad);
		iter = iter->next;
	}
    printf("\n****************************************\n");
}

void yazar_bilgi(YAZAR *head, int yazarID,KITAPYAZAR* kitapYazarHead,KITAP* kitapHead){
    //Bir yazarın bilgilerini ve ona ait kitapları gösteren fonksiyon
    if(head==NULL){
        printf("\nKayitli yazar yok!!!\n\n");
        return;
    }
    YAZAR *iter = head;
    while(iter->next != NULL&& iter->yazarID!=yazarID){
        iter = iter->next;
    }
    if(iter->yazarID==yazarID){
        printf("\n***************YAZAR BILGILERI***************\n");
        printf("\nYazar Id - Yazar Adi - Yazar Soyadi\n\n");
        printf("%d , %s , %s\n", iter->yazarID, iter->yazarAd, iter->yazarSoyad);
        printf("\n***************YAZARIN KITAPLARI***************\n");
        printf("\nKitap Adi - ISBN - Adet\n\n");
        KITAPYAZAR* kitapYazar = kitapYazarHead;
        while (kitapYazar!=NULL){
            if(kitapYazar->YazarID==yazarID){
                KITAP* kitap = kitapHead;
                while (kitap!=NULL){
                    if(strcmp(kitap->ISBN,kitapYazar->ISBN)==0){
                        printf("%s , %s , %d\n", kitap->kitapAdı, kitap->ISBN, kitap->adet);
                    }
                    kitap = kitap->next;
                }
            }
            kitapYazar = kitapYazar->next;
        }
        printf("\n***********************************************\n");
        
        
    }else{
        printf("\nBu id'ye sahip bir yazar bulunamadi!\n\n");
    }
}

void yazar_sil(YAZAR **head, int yazarID,KITAPYAZAR** kitapYazarHead){
    //Yazar silen fonksiyon
    if(*head==NULL){
        printf("\nKayitli yazar yok!!!\n\n");
        return;
    }
    YAZAR *iter = *head;
    YAZAR *temp = NULL;
    if(iter->yazarID==yazarID){
        temp = iter;
        *head = iter->next;
        kitapYazar_yazar_guncelle(kitapYazarHead,yazarID);
        free(temp);
        return;
    }
    while(iter->next != NULL && iter->next->yazarID!=yazarID){
        iter = iter->next;
    }
    if(iter->next==NULL){
        printf("\nBu id'ye sahip bir yazar bulunamadi!!!\n\n");
    }else if(iter->next->yazarID==yazarID){
        temp = iter->next;
        iter->next = iter->next->next;
        kitapYazar_yazar_guncelle(kitapYazarHead,yazarID);
        free(temp);
    }
}

void yazar_guncelle(YAZAR **head, int yazarID, char ad[], char soyad[]){
    //Yazar güncelleyen fonksiyon
    if(*head==NULL){
        printf("\nKayitli yazar yok!!!\n\n");
        return;
    }
	YAZAR *iter = *head;
	while(iter != NULL && iter->yazarID != yazarID){
		iter = iter->next;
	}
	if(iter != NULL){
		strcpy(iter->yazarAd, ad);
		strcpy(iter->yazarSoyad, soyad);
	}else{
        printf("\nBu id'ye sahip bir yazar bulunamadi!!!\n\n");
    }
}

void yazar_dosyaya_yaz(YAZAR *head){
	FILE *fp = fopen("Yazarlar.csv", "w");
    if (fp == NULL) {
        printf("\nYazar dosyasi acilamadi!!!\n");
        return;
    }
	YAZAR *iter = head;
	while(iter != NULL){
		fprintf(fp, "%d,%s,%s\n", iter->yazarID, iter->yazarAd, iter->yazarSoyad);
		iter = iter->next;
	}
	fclose(fp);
}

void yazar_dosyadan_oku(YAZAR** head){
    FILE *fp = fopen("Yazarlar.csv", "r");
    if (fp == NULL){
        printf("\nYazar dosyasi acilamadi!!!\n");
        return;
    }
    char line[50];
    char *field;
    char* ad;
    char* soyad;
    int yazarID;

    while(fgets(line, 50, fp)){
        field = strtok(line, ",");
        yazarID=atoi(field);
        field = strtok(NULL, ",");
        ad=field;
        field = strtok(NULL, "\n");
        soyad=field;
        yazar_ekle(head, ad, soyad,yazarID);
    }
    fclose(fp);
}

void ogrenci_free(OGRENCI** ogrenciHead){
    OGRENCI *temp = *ogrenciHead;
    while(*ogrenciHead != NULL){
        *ogrenciHead = (*ogrenciHead)->next;
        free(temp);
        temp = *ogrenciHead;
    }
}

void ogrenci_ekle(OGRENCI** ilk_ogrenci,OGRENCI** son_ogrenci,char ad[], char soyad[], int puan, char numara[]) {
    OGRENCI *ogrenci;
    ogrenci = (OGRENCI*)malloc(sizeof(OGRENCI));
    if(ogrenci==NULL){
        printf("\nOgrenci ekleme basarisiz!!!\n");
        return;
    }
    OGRENCI *iter = *ilk_ogrenci;
    while(iter != NULL && strcmp(iter->ogrID,numara)){
        iter = iter->next;
    }
    if(iter != NULL){
        printf("\nBu numaraya sahip bir ogrenci zaten var!!!\n\n");
        return;
    }
    strcpy(ogrenci->ad, ad);
    strcpy(ogrenci->soyad, soyad);
    ogrenci->puan = puan;
    strcpy(ogrenci->ogrID, numara);
    ogrenci->prev = NULL;
    ogrenci->next = NULL;

    if (*ilk_ogrenci == NULL) {
        *ilk_ogrenci = ogrenci;
        *son_ogrenci = ogrenci;
    }
    else {
        (*son_ogrenci)->next = ogrenci;
        ogrenci->prev = *son_ogrenci;
        *son_ogrenci = ogrenci;
    }
}

void ogrenci_dosyadan_oku(OGRENCI** ilk_ogrenci,OGRENCI** son_ogrenci) {
    FILE *fptr = fopen("Ogrenciler.csv", "r");
    if (fptr == NULL) {
        printf("\nOgrenci dosyasi acilamadi!!!\n");
        return;
    }
    char line[50];
    char *field;
    int i = 0;
    char* ad;
    char *soyad;
    char *numara;
    int puan;
    while(fgets(line, 50, fptr)){
        field = strtok(line, ",");
        numara=field;
        field = strtok(NULL, ",");
        ad=field;
        field = strtok(NULL, ",");
        soyad=field;
        field = strtok(NULL, "\n");
        puan=atoi(field);
        ogrenci_ekle(ilk_ogrenci,son_ogrenci,ad,soyad,puan,numara);
        i++;
    }
    fclose(fptr);
}

void ogrenci_dosyaya_yaz(OGRENCI* ilk_ogrenci) {
    FILE *fptr = fopen("Ogrenciler.csv", "w");
    if (fptr == NULL) {
        printf("\nOgrenci dosyasi acilamadi!!!\n");
        return;
    }
    OGRENCI *ogrenci = ilk_ogrenci;
    while (ogrenci != NULL) {
        fprintf(fptr, "%s,%s,%s,%d\n", ogrenci->ogrID, ogrenci->ad, ogrenci->soyad, ogrenci->puan);
        ogrenci = ogrenci->next;
    }
    fclose(fptr);
}

void ogrenci_guncelle(OGRENCI** ilk_ogrenci,char numara[], char ad[], char soyad[]) {
    OGRENCI *ogrenci = *ilk_ogrenci;
    if(ogrenci == NULL){
        printf("\nKayitli ogrenci yok!!!\n\n");
        return;
    }
    int found=0;
    while (ogrenci != NULL) {
        if (strcmp(ogrenci->ogrID, numara) == 0) {
            found=1;
            strcpy(ogrenci->ad, ad);
            strcpy(ogrenci->soyad, soyad);
            printf("Ogrencinin suanki puanı %d. Yeni puanini giriniz: ",ogrenci->puan);
            scanf("%d",&ogrenci->puan);
            return;
        }
        ogrenci = ogrenci->next;
    }
    if(found==0){
        printf("\nBu numaraya sahip ogrenci bulunamadi!!!\n\n");
    }
}

void ogrenci_sil(OGRENCI** ilk_ogrenci,OGRENCI** son_ogrenci,char numara[],KITAPODUNC **oduncHead, KITAPODUNC **oduncTail) {
    //Öğrenci silen fonksiyon
    if (*ilk_ogrenci==NULL){
        printf("\nKayitli ogrenci yok!!!\n\n");
        return;
    }
    int found=0;
    OGRENCI *ogrenci = *ilk_ogrenci;
    while (ogrenci != NULL) {
        if (strcmp(ogrenci->ogrID, numara) == 0) {
            kitap_odunc_sil(oduncHead,oduncTail,1,numara);//Öğrenci silindiğinde ilgili tüm ödünç işlemleri silinir
            if (ogrenci == *ilk_ogrenci) {
                *ilk_ogrenci = ogrenci->next;
            }
            else {
                ogrenci->prev->next = ogrenci->next;
            }

            if (ogrenci == *son_ogrenci) {
                *son_ogrenci = ogrenci->prev;
            }
            else {
                ogrenci->next->prev = ogrenci->prev;
            }
            found=1;
            free(ogrenci);
            break;
        }
        ogrenci = ogrenci->next;
    }
    if(found==0){
        printf("\nBu id'ye sahip bir ogrenci bulunamadi!!!\n\n");
    }
}

void ogrenci_listele(OGRENCI* ilk_ogrenci){
    //Tüm öğrencilerin bilgilerini listeleyen fonksiyon
    OGRENCI *ogrenci = ilk_ogrenci;
    if(ilk_ogrenci == NULL){
        printf("\nKayitli ogrenci yok!!!\n\n");
        return;
    }
    printf("\n***************OGRENCILER***************\n");
    printf("\nOgrenci Adi  -  Ogrenci Soyadi  -  Kutuphane Puani  -  Ogrenci No\n\n");
    while(ogrenci != NULL){
        printf("%s , %s , %d , %s\n", ogrenci->ad, ogrenci->soyad, ogrenci->puan, ogrenci->ogrID);
        ogrenci = ogrenci->next;
    }
    printf("\n****************************************\n");
}

void cezali_ogrencileri_listele(OGRENCI* ilk_ogrenci){
    //Kütüphane puanı 0'dan az olan öğrencilerin bilgilerini listeleyen fonksiyon
    OGRENCI *ogrenci = ilk_ogrenci;
    if(ilk_ogrenci == NULL){
        printf("\nKayitli ogrenci yok!!!\n\n");
        return;
    }
    printf("\n***************CEZALI OGRENCILER***************\n");
    printf("\nOgrenci Adi  -  Ogrenci Soyadi  -  Kutuphane Puani  -  Ogrenci No\n\n");
    while(ogrenci != NULL){
        if(ogrenci->puan<=0){
            printf("%s , %s , %d , %s\n", ogrenci->ad, ogrenci->soyad, ogrenci->puan, ogrenci->ogrID);
        }
        ogrenci = ogrenci->next;
    }
    printf("\n****************************************\n");
}

void kitap_teslim_etmemis_ogrencileri_listele(OGRENCI* ilk_ogrenci,KITAP* kitapHead){
    //Kitap teslim etmemiş öğrencilerin bilgilerini listeleyen fonksiyon
    if(kitapHead==NULL){
        printf("\nKayitli kitap yok!!!\n\n");
        return;
    }
    if(ilk_ogrenci==NULL){
        printf("\nKayitli ogrenci yok!!!\n\n");
        return;
    }
    KITAP *iter=kitapHead;
    KITAPORNEK *ornek;
    OGRENCI* ogrenci;
    printf("\n***************KITAP TESLIM ETMEMIS OGRENCILER***************\n");
    printf("\nOgrenci Adi  -  Ogrenci Soyadi  -  Kutuphane Puani  -  Ogrenci No - Kitap Adi - Kitap Etiket No\n\n");
    while(iter!=NULL){
        ornek=iter->head;
        while(ornek!=NULL){
            if(strcmp(ornek->Durum,"RAFTA")){
                ogrenci = ogrenci_bul(ilk_ogrenci,ornek->Durum);
                printf("%s , %s , %d , %s , %s , %s\n",ogrenci->ad, ogrenci->soyad, ogrenci->puan, ogrenci->ogrID,iter->kitapAdı,ornek->EtiketNo);
            }
            ornek=ornek->next;
        }
        iter=iter->next;
    }
    printf("\n****************************************\n");
}

void ogrenci_bilgi(OGRENCI* ilk_ogrenci,char id[]){
    //Tek bir öğrencinin bilgilerini gösteren fonksiyon
    if(ilk_ogrenci == NULL){
        printf("\nKayitli ogrenci yok!!!\n\n");
        return;
    }
    OGRENCI *ogrenci = ilk_ogrenci;
    
    while(ogrenci->next != NULL&& strcmp(ogrenci->ogrID, id)!=0){
        ogrenci = ogrenci->next;
    }
    if(strcmp(ogrenci->ogrID, id)==0){
        printf("\n***************OGRENCI BILGILERI***************\n");
        printf("\nOgrenci Adi - Ogrenci Soyadi - Kutuphane Puani - Ogrenci No\n\n");
        printf("%s , %s , %d , %s\n", ogrenci->ad, ogrenci->soyad, ogrenci->puan, ogrenci->ogrID);
        printf("\n***********************************************\n");
    }else{
        printf("\nBu id'ye sahip bir ogrenci bulunamadi!!!\n\n");
    }  
}

int main(){
    int (*fptr[2])(KITAP*,char[])= {kitapAdKarsilastir, kitapIsbnKarsilastir};
    YAZAR *yazarHead = NULL;
    KITAP *kitapHead = NULL;
    KITAPYAZAR *kitapYazarHead = NULL;
    KITAPODUNC *kitapOduncHead = NULL;
    KITAPODUNC *kitapOduncTail = NULL;
    OGRENCI *ilk_ogrenci = NULL;
    OGRENCI *son_ogrenci = NULL;
    int anaIslem,islem,exit=0,id,adet,aramaTipi;
    char ad[30],soyad[30],ogrId[9],ISBN[14],arananKitap[20];
    //Program başında tüm csv dosyalarından okuma işlemleri yapılır
    kitapOdunc_dosyadan_oku(&kitapOduncHead,&kitapOduncTail);
    yazar_dosyadan_oku(&yazarHead);
    ogrenci_dosyadan_oku(&ilk_ogrenci,&son_ogrenci);
    kitap_dosyadan_oku(&kitapHead,&kitapOduncTail);
    kitapYazar_dosyadan_oku(&kitapYazarHead,kitapHead,yazarHead);

    while(exit==0){
        do{
            printf("\nOgrenci islemleri icin 1'e basiniz\n");
            printf("Kitap islemleri icin 2'ye basiniz\n");
            printf("Yazar islemleri icin 3'e basiniz\n");
            printf("Cıkıs yapmak icin 4'e basiniz\n");
            scanf("%d",&anaIslem);
        }
        while(anaIslem<1&&anaIslem>4);

        switch (anaIslem){
            case 1:
                do{
                    printf("\nOgrenci eklemek icin 1'e basiniz\n");
                    printf("Ogrenci silmek icin 2'ye basiniz\n");
                    printf("Ogrenci güncellemek icin 3'e basiniz\n");
                    printf("Ogrenci bilgisi göstermek icin 4'e basiniz\n");
                    printf("Kitap teslim etmemis ogrencileri listelemek icin 5'e basiniz\n");
                    printf("Cezalı ogrencileri listelemek icin 6'ya basiniz\n");
                    printf("Tum ogrencileri listelemek icin 7'ye basiniz\n");
                    printf("Kitap odunc almak icin 8'e basiniz\n");
                    printf("Kitap teslim etmek icin 9'a basiniz\n");
                    printf("Geri gelmek icin 10'a basiniz\n");
                    scanf("%d",&islem);
                }
                while(islem<1&&islem>10);
                switch (islem){
                    case 1:
                        printf("Ogrenci adi giriniz: ");
                        scanf("%s",ad);
                        printf("Ogrenci soyadi giriniz: ");
                        scanf("%s",soyad);
                        printf("Ogrenci numarasi giriniz: ");
                        scanf("%s",ogrId);
                        ogrenci_ekle(&ilk_ogrenci,&son_ogrenci,ad,soyad,100,ogrId);
                        break;
                    case 2:
                        printf("Ogrenci numarasini giriniz: ");
                        scanf("%s",ogrId);
                        ogrenci_sil(&ilk_ogrenci,&son_ogrenci,ogrId,&kitapOduncHead,&kitapOduncTail);
                        break;
                    case 3:
                        printf("Güncellemek istediginiz ögrencinin numarasini giriniz: ");
                        scanf("%s",ogrId);
                        printf("Ogrencinin yeni adini giriniz: ");
                        scanf("%s",ad);
                        printf("Ogrencinin yeni soyadini giriniz: ");
                        scanf("%s",soyad);
                        ogrenci_guncelle(&ilk_ogrenci,ogrId,ad,soyad);
                        break;
                    case 4:
                        printf("Ogrenci numarasini giriniz: ");
                        scanf("%s",ogrId);
                        ogrenci_bilgi(ilk_ogrenci,ogrId);
                        break;
                    case 5:
                        kitap_teslim_etmemis_ogrencileri_listele(ilk_ogrenci,kitapHead);
                        break;
                    case 6:
                        cezali_ogrencileri_listele(ilk_ogrenci);
                        break;
                    case 7:
                        ogrenci_listele(ilk_ogrenci);
                        break;
                    case 8:
                        printf("Kitap arama tipi giriniz   Kitap Adı-1    ISBN-2   : ");
                        scanf("%d",&aramaTipi);
                        kitap_odunc_al(&kitapHead,&kitapOduncHead,&kitapOduncTail, ilk_ogrenci,fptr[aramaTipi-1]);
                        break;
                    case 9:
                        printf("Kitap arama tipi giriniz   Kitap Adı-1    ISBN-2   : ");
                        scanf("%d",&aramaTipi);
                        kitap_teslim_et(&kitapHead,&kitapOduncHead,&kitapOduncTail,&ilk_ogrenci,fptr[aramaTipi-1]);
                        break;
                    case 10:
                        break;
                    
                    default:
                        printf("Gecersiz islem!!!\n");
                        break;
                }
                break;
            case 2:
                do{
                    printf("\nKitap eklemek icin 1'e basiniz\n");
                    printf("Kitap silmek icin 2'ye basiniz\n");
                    printf("Kitap güncellemek icin 3'e basiniz\n");
                    printf("Kitap bilgisi göstermek icin 4'e basiniz\n");
                    printf("Raftaki kitaplari listelemek icin 5'e basiniz\n");
                    printf("Zamaninda teslim edilmeyen kitaplari listelemek icin 6'ya basiniz\n");
                    printf("Kitap-yazar eslestirmek icin 7'ye basiniz\n");
                    printf("Kitap yazari güncellemek icin 8'e basiniz\n");
                    printf("Kitap-yazar eslestirmelerini listelemek icin 9'a basiniz\n");
                    printf("Geri gelmek icin 10'a basiniz\n");
                    scanf("%d",&islem);
                }
                while(islem<1&&islem>10);
                switch (islem){
                    case 1:
                        printf("Kitap ismi giriniz: ");
                        scanf(" %[^\n]s",ad);
                        printf("Kitap ISBN giriniz: ");
                        scanf("%s",ISBN);
                        printf("Kitap adet giriniz: ");
                        scanf("%d",&adet);
                        kitap_ekle(&kitapHead,ad,ISBN,adet,0,&kitapOduncTail);
                        break;
                    case 2:
                        printf("Arama tipi giriniz   Kitap Adı-1    ISBN-2   : ");
                        scanf("%d",&aramaTipi);
                        kitap_sil(&kitapHead,fptr[aramaTipi-1],&kitapOduncHead,&kitapOduncTail,&kitapYazarHead);
                        break;
                    case 3:
                        printf("Arama tipi giriniz   Kitap Adı-1    ISBN-2   : ");
                        scanf("%d",&aramaTipi);
                        kitap_guncelle(&kitapHead,fptr[aramaTipi-1]);
                        break;
                    case 4:
                        printf("Arama tipi giriniz   Kitap Adı-1    ISBN-2   : ");
                        scanf("%d",&aramaTipi);
                        kitap_bilgi(kitapHead,fptr[aramaTipi-1],yazarHead,kitapYazarHead);
                        break;
                    case 5:
                        kitap_listele(kitapHead);
                        break;
                    case 6:
                        zamaninda_teslim_edilmeyen_kitaplar_listele(kitapOduncHead,kitapHead,ilk_ogrenci);
                        break;
                    case 7:
                        printf("Kitap arama tipi giriniz   Kitap Adı-1    ISBN-2   : ");
                        scanf("%d",&aramaTipi);
                        printf("Aramak istediginiz kitap bilgisini giriniz: ");
                        scanf(" %[^\n]s",arananKitap);
                        printf("Aramak istediginiz yazarin ID'sini giriniz: ");
                        scanf("%d",&id);
                        kitapYazar_ekle(kitapHead,yazarHead,&kitapYazarHead,fptr[aramaTipi-1],arananKitap,id);
                        break;
                    case 8:
                        kitapYazar_guncelle(&kitapYazarHead,yazarHead);
                        break;
                    case 9:
                        kitapYazar_listele(kitapYazarHead);
                        break;
                    case 10:
                        break;
                    default:
                        printf("Gecersiz islem!!!\n");
                        break;
                }
                break;
            case 3:
                do{
                    printf("\nYazar eklemek icin 1'e basiniz\n");
                    printf("Yazar silmek icin 2'ye basiniz\n");
                    printf("Yazar güncellemek icin 3'e basiniz\n");
                    printf("Yazar bilgisi göstermek icin 4'e basiniz\n");
                    printf("Tum yazarlari listelemek icin 5'e basiniz\n");
                    printf("Geri gelmek icin 6'ya basiniz\n");
                    scanf("%d",&islem);
                }
                while(islem<1&&islem>6);
                switch (islem){
                    case 1:
                        printf("Yazar adi giriniz: ");
                        scanf("%s",ad);
                        printf("Yazar soyadi giriniz: ");
                        scanf("%s",soyad);
                        yazar_ekle(&yazarHead,ad,soyad,0);
                        break;
                    case 2:
                        printf("Yazar ID giriniz: ");
                        scanf("%d",&id);
                        yazar_sil(&yazarHead,id,&kitapYazarHead);
                        break;
                    case 3:
                        printf("Güncellemek istediginiz yazarin ID'sini giriniz: ");
                        scanf("%d",&id);
                        printf("Yeni yazar adi giriniz: ");
                        scanf("%s",ad);
                        printf("Yeni yazar soyadi giriniz: ");
                        scanf("%s",soyad);
                        yazar_guncelle(&yazarHead,id,ad,soyad);
                        break;
                    case 4:
                        printf("Yazar ID giriniz: ");
                        scanf("%d",&id);
                        yazar_bilgi(yazarHead,id,kitapYazarHead,kitapHead);
                        break;
                    case 5:
                        yazar_listele(yazarHead);
                        break;
                    case 6:
                        break;

                    default:
                        printf("Gecersiz islem!!!\n");
                        break;
                }
                break;
            case 4:
                exit=1;
                break;
            
            default:
                printf("Gecersiz islem!!!\n");
                break;
        }
    }
    //Program kapanırken tüm linked listler csv dosyalarına yazılır
    yazar_dosyaya_yaz(yazarHead);
    ogrenci_dosyaya_yaz(ilk_ogrenci);
    kitap_dosyaya_yaz(kitapHead);
    kitapOdunc_dosyaya_yaz(kitapOduncHead);
    kitapYazar_dosyaya_yaz(kitapYazarHead);
    //Program kapanırken tüm linked listler free edilir
    yazar_free(&yazarHead);
    ogrenci_free(&ilk_ogrenci);
    kitap_free(&kitapHead);
    kitap_odunc_free(&kitapOduncHead);
    kitapYazar_free(&kitapYazarHead);
    printf("Programdan cikilmistir!!!\n");
    return 0;
}