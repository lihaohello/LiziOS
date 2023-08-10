char message[] = "hello";

int main() {
    char* video = (char*)0xb8000;

    for (int i = 0; i < sizeof(message); i++) {
        *video = message[i];
        video+=2;
    }

    return 0;
}
