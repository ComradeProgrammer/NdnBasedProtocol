
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include<iostream>
#include<string>
#define KEY_LENGTH  2048
using namespace std;
int main(){
    size_t pri_len;  
    size_t pub_len;  
    char *pri_key = NULL;  
    char *pub_key = NULL; 

    auto bne = BN_new();
    auto ret = BN_set_word(bne,RSA_F4);

    RSA *keypair = RSA_new();
    ret = RSA_generate_key_ex(keypair, KEY_LENGTH, bne, NULL);  

    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem()); 
    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);  
    PEM_write_bio_RSAPublicKey(pub, keypair);  

    pri_len = BIO_pending(pri);  
    pub_len = BIO_pending(pub); 

    pri_key = new char[pri_len + 1];  
    pub_key =  new char [pub_len + 1];  

    ret=BIO_read(pri, pri_key, pri_len);  
    BIO_read(pub, pub_key, pub_len); 
  
    pri_key[pri_len] = '\0';  
    pub_key[pub_len] = '\0';  

    string priKeyString=string(pri_key);
    string pubKeyString=string(pub_key);

    cout<<priKeyString<<endl<<endl;
    cout<<pubKeyString<<endl<<endl;
    cout<<pubKeyString.size()<<endl;

    const char* test="helloworldhelloworldhelloworldhelloworldhello worldhelloworldhelloworldhellfffffffffffffffffffffffffffffffffffffffffffffffffff";
    char* testout=new char [2048];


    BIO* in1 = BIO_new_mem_buf((void*)priKeyString.c_str(), -1);
    
    RSA* rsa1 = PEM_read_bio_RSAPrivateKey(in1, NULL, NULL, NULL);

    ret=RSA_private_encrypt(116,(unsigned char*)(test),(unsigned char*)(testout),rsa1, RSA_PKCS1_PADDING);

    BIO* in = BIO_new_mem_buf((void*)pubKeyString.c_str(), -1);
    
    RSA* rsa2 = PEM_read_bio_RSAPublicKey(in, NULL, NULL, NULL);
    cout<<rsa2<<endl;

    char* testout2=new char [2048];
    int ret2=RSA_public_decrypt(ret,(unsigned char*)(testout),(unsigned char*)(testout2),rsa2,RSA_PKCS1_PADDING);
    cout<<testout2<<endl;


    BIO_free_all(pub);  
    BIO_free_all(pri);  
    RSA_free(keypair);
    BN_free(bne);

    return 0;
}