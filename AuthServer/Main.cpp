#include <NGin/Network/Network.h>

typedef uint64_t guid_t;
const int GUID_L = 64;

struct User {
  guid_t guid;
  string name;
  string passSalt; //TODO: Don't just store plaintext
  string passHash; //TODO: Don't just store plaintext
};

void loadUsers() {

}

bool verifyUser(string userId, string password, string& guid) {
  cout << "Verified " << userId << ", " << password << endl;
  guid = "123456789ABCDEF";
  return true;
}

bool mainRecieve(DataElement* Data, int Id, Network* thisptr, NetworkBinder* server) {
  if (Id != 1) {
    return true;
  }

  int protocol;
  vSFunc<int>(protocol, Data->_children[0]);

  string userId;
  vSFunc<string>(userId, Data->_children[1]);

  string password;
  vSFunc<string>(password, Data->_children[2]);

  string guid;
  bool result = verifyUser(userId, password, guid);

  DataElement* res = new DataElement();

  DataElement* res_e = new DataElement();
  vGFunc<bool>(result, res_e);
  res->addChild(res_e);

  DataElement* res_g = new DataElement();
  vGFunc<string>(guid, res_g);
  res->addChild(res_g);

  thisptr->SendData(res, 1);

  return true;
}

void main() {
  while (true) {
    NetworkS server("7742", mainRecieve);
    server.ReciveLoopThread.join();
  }
}