

#include "CHOP_CPlusPlusBase.h"

using namespace TD;

//関数の詳細については CHOP_CPlusPlusBase.h を参照
//何か全角記号をつけるとここでだけエラー吐くので注意...
class LipSyncCHOP : public CHOP_CPlusPlusBase
{
public:
  LipSyncCHOP(const OP_NodeInfo* info);
  virtual ~LipSyncCHOP();

  virtual void getGeneralInfo(CHOP_GeneralInfo*, const OP_Inputs*, void*) override;
  virtual bool getOutputInfo(CHOP_OutputInfo*, const OP_Inputs*, void*) override;
  virtual void getChannelName(int32_t index, OP_String* name, const OP_Inputs*, void* reserved) override;

  virtual void execute(CHOP_Output*, const OP_Inputs*, void* reserved) override;


  virtual int32_t getNumInfoCHOPChans(void* reserved1) override;
  virtual void getInfoCHOPChan(int index, OP_InfoCHOPChan* chan, void* reserved1) override;

  virtual bool getInfoDATSize(OP_InfoDATSize* infoSize, void* resereved1) override;
  virtual void getInfoDATEntries(int32_t index, int32_t nEntries, OP_InfoDATEntries* entries, void* reserved1) override;

  virtual void setupParameters(OP_ParameterManager* manager, void* reserved1) override;
  virtual void pulsePressed(const char* name, void* reserved1) override;

private:
  //OP_NodeInfoクラス：このクラスのインスタンスを使用しているノードに関する情報（名前など）を格納する.
  const OP_NodeInfo* myNodeInfo;

  //execute()が呼び出される度にインクリメント.
  int32_t myExecuteCount;

  double myOffset;
};
