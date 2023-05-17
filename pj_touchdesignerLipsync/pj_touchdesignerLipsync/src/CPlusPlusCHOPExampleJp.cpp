/*
- このファイルはDerivative inc.が所有している.
- TouchDesignerと共に使用若しくは使用するために修正する場合にのみ使用可能.
- 以下の場合につき、このファイルの修正版を共有またば再配布が可能.
 　- Derivative inc.の商標を使用しない.
 　- このファイルから派生した製品の推奨や宣伝に使用しない.
*/

#include "CPlusPlusCHOPExampleJp.h"

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <assert.h>

// コンパイラがこれらの関数を.dllからエクスポートするために必要.
extern "C" {
  DLLEXPORT
  void FillCHOPPluginInfo(CHOP_PluginInfo* info)
  {
      //CHOPCPlusPlusAPIVersion の設定.
      info->apiVersion = CHOPCPlusPlusAPIVersion;

      //OP名の設定
      //最初が大文字かつそれ以降は小文字でなければならない.
      info->customOPInfo.opType->setString("Examplejp");

      //OPDialog上でのOP名
      info->customOPInfo.opLabel->setString("ExampleJP");

      //Author情報
      info->customOPInfo.authorName->setString("Author Name");
      info->customOPInfo.authorEmail->setString("email@email.com");

      //動作する最低インプット数/
      info->customOPInfo.minInputs = 0;

      //動作する最高インプット数
      info->customOPInfo.maxInputs = 1;
  }

  //Touch上で新規OP作成をした際に呼び出される.
  DLLEXPORT
  CHOP_CPlusPlusBase* CreateCHOPInstance(const OP_NodeInfo* info)
  {
      return new CPlusPlusCHOPExampleJp(info);
  }

  //作成したOPを削除した時、Touchをシャットダウンした時、CHOPが別のdllをロードした時に呼び出される.
  DLLEXPORT
  void DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
  {
    delete (CPlusPlusCHOPExampleJp*)instance;
  }
};


CPlusPlusCHOPExampleJp::CPlusPlusCHOPExampleJp(const OP_NodeInfo* info) : myNodeInfo(info)
{
    myExecuteCount = 0;
    myOffset = 0.0;
}

CPlusPlusCHOPExampleJp::~CPlusPlusCHOPExampleJp()
{
}

void CPlusPlusCHOPExampleJp::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
    //変更があったら毎フレーム処理
    ginfo->cookEveryFrameIfAsked = true;
    //timesliceを有効化
    ginfo->timeslice = true;

    ginfo->inputMatchIndex = 0;
}

bool CPlusPlusCHOPExampleJp::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs* inputs, void* reserved1)
{
    //入力チャンネル数によって処理を変化
    if (inputs->getNumInputs() > 0)
    {
        return false;
    }
    else
    {
        info->numChannels = 1;
        info->sampleRate = 120;
        return true;
    }
}

void CPlusPlusCHOPExampleJp::getChannelName(int32_t index, OP_String* name, const OP_Inputs* inputs, void* reserved1)
{
    name->setString("chan1");
}

void CPlusPlusCHOPExampleJp::execute(CHOP_Output* output, const OP_Inputs* inputs, void* reserved)
{
    myExecuteCount++;

    double scale = inputs->getParDouble("Scale");

    if (inputs->getNumInputs() > 0) //入力があれば：それをスケールする.
    {
        inputs->enablePar("Speed", 0); // not used
        inputs->enablePar("Reset", 0); // not used
        inputs->enablePar("Shape", 0); // not used

        int ind = 0;
        const OP_CHOPInput* cinput = inputs->getInputCHOP(0);

        for (int i = 0; i < output->numChannels; i++)
        {
            for (int j = 0; j < output->numSamples; j++)
            {
                output->channels[i][j] = float(cinput->getChannelData(i)[ind] * scale);
                ind++;

                // CHOPの入力の最後を読み切らない.
                ind = ind % cinput->numSamples;
            }
        }
    }
    else //入力がなければ：sin波を出力する.
    {
        inputs->enablePar("Speed", 1);
        inputs->enablePar("Reset", 1);

        double speed = inputs->getParDouble("Speed");
        double step = speed * 0.01f;


        int shape = inputs->getParInt("Shape"); //メニュー項目
        //const char *shape_str = inputs->getParString("Shape");

        //各チャンネルを異なる位相に保つ.
        double phase = 2.0f * 3.14159f / (float)(output->numChannels);


        for (int i = 0; i < output->numChannels; i++)
        {
            double offset = myOffset + phase * i;
            double v = 0.0f;

            switch (shape)
            {
            case 0: // sine
                v = sin(offset);
                break;

            case 1: // square
                v = fabs(fmod(offset, 1.0)) > 0.5;
                break;

            case 2: // ramp
                v = fabs(fmod(offset, 1.0));
                break;
            }

            v *= scale;

            for (int j = 0; j < output->numSamples; j++)
            {
                output->channels[i][j] = float(v);
                offset += step;
            }
        }
        myOffset += step * output->numSamples;
    }
}

int32_t CPlusPlusCHOPExampleJp::getNumInfoCHOPChans(void* reserved1)
{
    //任意のInfoCHOPに出力したいチャンネル数を返す.
    return 2;
}

//返したい各チャンネルに対して1回呼び出される（この例では一回のみ）.
void CPlusPlusCHOPExampleJp::getInfoCHOPChan(int32_t index, OP_InfoCHOPChan* chan, void* reserved1)
{
    if (index == 0)
    {
        chan->name->setString("executeCount");
        chan->value = (float)myExecuteCount;
    }

    if (index == 1)
    {
        chan->name->setString("offset");
        chan->value = (float)myOffset;
    }
}

bool CPlusPlusCHOPExampleJp::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
    infoSize->rows = 2;
    infoSize->cols = 2;

    //これをfalseにすると、テーブルに割り当てることになる.
    //trueにすると、一度に一列ずつ行う.
    infoSize->byColumn = false;
    return true;
}

void CPlusPlusCHOPExampleJp::getInfoDATEntries(int32_t index, int32_t nEntries, OP_InfoDATEntries* entries, void* reserved1)
{
    char tempBuffer[4096];

    if (index == 0)
    {
        //1列目のカラムに値を設定する.
        entries->values[0]->setString("executeCount");

        //2列目のカラムに値を設定する.
#ifdef _WIN32
        sprintf_s(tempBuffer, "%d", myExecuteCount);
#else // macOS
        snprintf(tempBuffer, sizeof(tempBuffer), "%d", myExecuteCount);
#endif
        entries->values[1]->setString(tempBuffer);
    }

    if (index == 1)
    {
        //1列目のカラムに値を設定する.
        entries->values[0]->setString("offset");

        //2列目のカラムに値を設定する.
#ifdef _WIN32
        sprintf_s(tempBuffer, "%g", myOffset);
#else // macOS
        snprintf(tempBuffer, sizeof(tempBuffer), "%g", myOffset);
#endif
        entries->values[1]->setString(tempBuffer);
    }
}

void CPlusPlusCHOPExampleJp::setupParameters(OP_ParameterManager* manager, void* reserved1)
{
    // speed
    {
        OP_NumericParameter np;

        np.name = "Speed";
        np.label = "Speed";
        np.defaultValues[0] = 1.0;
        np.minSliders[0] = -10.0;
        np.maxSliders[0] = 10.0;

        OP_ParAppendResult res = manager->appendFloat(np);
        assert(res == OP_ParAppendResult::Success);
    }
    // scale
    {
        OP_NumericParameter np;

        np.name = "Scale";
        np.label = "Scale";
        np.defaultValues[0] = 1.0;
        np.minSliders[0] = -10.0;
        np.maxSliders[0] = 10.0;

        OP_ParAppendResult res = manager->appendFloat(np);
        assert(res == OP_ParAppendResult::Success);
    }
    // shape
    {
        OP_StringParameter sp;

        sp.name = "Shape";
        sp.label = "Shape";

        sp.defaultValue = "Sine";

        const char* names[] = { "Sine", "Square", "Ramp" };
        const char* labels[] = { "Sine", "Square", "Ramp" };

        OP_ParAppendResult res = manager->appendMenu(sp, 3, names, labels);
        assert(res == OP_ParAppendResult::Success);
    }
    // pulse
    {
        OP_NumericParameter np;

        np.name = "Reset";
        np.label = "Reset";

        OP_ParAppendResult res = manager->appendPulse(np);
        assert(res == OP_ParAppendResult::Success);
    }
}

void CPlusPlusCHOPExampleJp::pulsePressed(const char* name, void* reserved1)
{
    if (!strcmp(name, "Reset"))
    {
        myOffset = 0.0;
    }
}
