#include "easypr.h"
#include "easypr/util/switch.hpp"

#include "accuracy.hpp"
#include "chars.hpp"
#include "plate.hpp"

namespace easypr {

namespace demo {

// interactions

int accuracyTestMain() {
  bool isExit = false;
  while (!isExit) {
    std::cout << "////////////////////////////////////" << std::endl;
    const char* options[] = {"BatchTest Option:", "1. general_test;",
                             "2. native_test;", "3. ����;", NULL};
    Utils::print_str_lines(options);
    std::cout << "////////////////////////////////////" << std::endl;
    std::cout << "��ѡ��һ�����:";

    int select = -1;
    bool isRepeat = true;
    while (isRepeat) {
      std::cin >> select;
      isRepeat = false;
      switch (select) {
        case 1:
          accuracyTest("resources/image/general_test");
          break;
        case 2:
          accuracyTest("resources/image/native_test");
          break;
        case 3:
          isExit = true;
          break;
        default:
          std::cout << "�����������������:";
          isRepeat = true;
          break;
      }
    }
  }
  return 0;
}

int testMain() {
  bool isExit = false;
  while (!isExit) {
    std::cout << "////////////////////////////////////" << std::endl;
    const char* options[] = {
        "EasyPR Test:", "1. test plate_locate(���ƶ�λ);" /* ���ƶ�λ */,
        "2. test plate_judge(�����ж�);" /* �����ж� */,
        "3. test plate_detect(���Ƽ��);" /* ���Ƽ�⣨�������ƶ�λ�복���жϣ� */,
        "4. test chars_segment(�ַ��ָ�);" /* �ַ��ָ� */,
        "5. test chars_identify(�ַ�����);" /* �ַ����� */,
        "6. test chars_recognise(�ַ�ʶ��);" /* �ַ�ʶ�𣨰����ַ��ָ����ַ����� */,
        "7. test plate_recognize(����ʶ��);" /* ����ʶ�� */,
        "8. test all(����ȫ��);" /* ����ȫ�� */, "9. ����;" /* �˳� */, NULL};
    Utils::print_str_lines(options);
    std::cout << "////////////////////////////////////" << std::endl;
    std::cout << "��ѡ��һ�����:";

    int select = -1;
    bool isRepeat = true;
    while (isRepeat) {
      std::cin >> select;
      isRepeat = false;
      switch (select) {
        case 1:
          assert(test_plate_locate() == 0);
          break;
        case 2:
          assert(test_plate_judge() == 0);
          break;
        case 3:
          assert(test_plate_detect() == 0);
          break;
        case 4:
          assert(test_chars_segment() == 0);
          break;
        case 5:
          assert(test_chars_identify() == 0);
          break;
        case 6:
          assert(test_chars_recognise() == 0);
          break;
        case 7:
          assert(test_plate_recognize() == 0);
          break;
        case 8:
          assert(test_plate_locate() == 0);
          assert(test_plate_judge() == 0);
          assert(test_plate_detect() == 0);

          assert(test_chars_segment() == 0);
          assert(test_chars_identify() == 0);
          assert(test_chars_recognise() == 0);

          assert(test_plate_recognize() == 0);
          break;
        case 9:
          isExit = true;
          break;
        default:
          std::cout << "�����������������:";
          isRepeat = true;
          break;
      }
    }
  }

  return 0;
}

}  // namespace demo

}  // namespace easypr

void command_line_handler(int argc, const char* argv[]) {
  program_options::Generator options;

  options.add_subroutine("svm", "svm operations").make_usage("Usage:");
  {
    /* ------------------------------------------
     | SVM Training operations
     | ------------------------------------------
     |
     | $ demo svm --plates=path/to/plates/ [--test] --svm=save/to/svm.xml
     |
     | ------------------------------------------
     */
    options("h,help", "show help information");
    options(",plates", "",
            "a folder contains both forward data and inverse data in the "
            "separated subfolders");
    options(",svm", easypr::kDefaultSvmPath, "the svm model file");
    options("t,test", "run tests in --plates");
  }

  options.add_subroutine("ann", "ann operation").make_usage("Usages:");
  {
    /* ------------------------------------------
    | ANN_MLP Training operations
    | ------------------------------------------
    |
    | $ demo ann --zh-chars=zhchars/ --en-chars=enchars/ --ann=save/to/ann.xml
    |
    | ------------------------------------------
    */
    options("h,help", "show help information");
    options(",chars", "",
            "the folder contains character sub-folders, with each folder"
            "named by label defined in include/easypr/config.h");
    options(",ann", easypr::kDefaultAnnPath,
            "the ann model file you want to save");
    options("t,test", "run test in --chars");
  }

  options.add_subroutine("locate", "locate plates in an image")
      .make_usage("Usage:");
  {
    /* ------------------------------------------
    | Plate locating operations
    | ------------------------------------------
    |
    | $ demo locate -f file
    |
    | ------------------------------------------
    */
    options("h,help", "show help information");
    options("f,file", "",
            "the target picture which contains one or more plates");
  }

  options.add_subroutine(
             "judge", "determine whether an image block is the license plate")
      .make_usage("Usage:");
  {
    /* ------------------------------------------
    | Plate judge operations
    | ------------------------------------------
    |
    | $ demo judge -f file --svm resources/model/svm.xml
    |
    | ------------------------------------------
    */
    options("h,help", "show help information");
    options("f,file", "the target image block");
    options(",svm", easypr::kDefaultSvmPath, "the svm model file");
  }

  options.add_subroutine("recognize", "plate recognition").make_usage("Usage:");
  {
    /* ------------------------------------------
    | Plate recognize operations
    | ------------------------------------------
    |
    | $ demo recognize -p file --svm resources/model/svm.xml
    |                          --ann resources/model/ann.xml
    | $ demo recognize -pb dir/ --svm resources/model/svm.xml
    |                           --ann resources/model/ann.xml
    |
    | ------------------------------------------
    */
    options("h,help", "show help information");
    options("p,path", "", "where is the target picture or target folder");
    options("b,batch", "do batch recognition, if set, --path means a folder");
    options("c,color", "returns the plate color, blue or yellow");
    options(",svm", easypr::kDefaultSvmPath, "the svm model file");
    options(",ann", easypr::kDefaultAnnPath, "the ann model file");
  }

  auto parser = options.make_parser();

  try {
    parser->parse(argc, argv);
  } catch (const std::exception& err) {
    std::cout << err.what() << std::endl;
    return;
  }

  auto subname = parser->get_subroutine_name();

  program_options::select(subname)
      .found("svm",
             [&]() {
               if (parser->has("help") || argc <= 2) {
                 std::cout << options("svm");
                 return;
               }

               easypr::SvmTrain svm(parser->get("plates")->c_str(),
                                    parser->get("svm")->c_str());

               if (parser->has("test")) {
                 svm.test();
               } else {
                 svm.train();
               }
             })
      .found("ann",
             [&]() {
               if (parser->has("help") || argc <= 2) {
                 std::cout << options("ann");
                 return;
               }

               assert(parser->has("chars"));
               assert(parser->has("ann"));

               easypr::AnnTrain ann(parser->get("chars")->c_str(),
                                    parser->get("ann")->c_str());

               if (parser->has("test")) {
                 ann.test();
               } else {
                 ann.train();
               }
             })
      .found("locate",
             [&]() {
               if (parser->has("help") || argc <= 2) {
                 std::cout << options("locate");
                 return;
               }

               if (parser->has("file")) {
                 easypr::api::plate_locate(parser->get("file")->val().c_str());
                 std::cout << "finished, results can be found in tmp/"
                           << std::endl;
               }
             })
      .found("judge",
             [&]() {
               if (parser->has("help") || argc <= 2) {
                 std::cout << options("judge");
                 std::cout << "Note that the input image's size should "
                           << "be the same as the one you gived to svm train."
                           << std::endl;
                 return;
               }

               if (parser->has("file")) {
                 assert(parser->has("file"));
                 assert(parser->has("svm"));

                 auto image = parser->get("file")->val();
                 auto svm = parser->get("svm")->val();

                 const char* true_or_false[2] = {"false", "true"};

                 std::cout << true_or_false[easypr::api::plate_judge(
                                  image.c_str(), svm.c_str())]
                           << std::endl;
               }
             })
      .found("recognize",
             [&]() {
               if (parser->has("help") || argc <= 2) {
                 std::cout << options("recognize");
                 return;
               }

               if (parser->has("path")) {
                 if (parser->has("batch")) {
                   // batch testing
                   auto folder = parser->get("path")->val();
                   easypr::demo::accuracyTest(folder.c_str());
                 } else {
                   // single testing
                   auto image = parser->get("path")->val();

                   if (parser->has("color")) {
                     // return plate color
                     const char* colors[2] = {"blue", "yellow"};
                     std::cout
                         << colors[easypr::api::get_plate_color(image.c_str())]
                         << std::endl;
                   } else {
                     // return strings
                     auto svm = parser->get("svm")->val();
                     auto ann = parser->get("ann")->val();

                     auto results = easypr::api::plate_recognize(
                         image.c_str(), svm.c_str(), ann.c_str());
                     for (auto s : results) {
                       std::cout << s << std::endl;
                     }
                   }
                 }
               } else {
                 std::cout << "option 'file' cannot be empty." << std::endl;
               }
             })
      .others([&]() {
        // no case matched, print all commands.
        std::cout << "There are several sub commands listed below, "
                  << "choose one by typing:\n\n"
                  << "    " << easypr::utils::getFileName(argv[0])
                  << " command [options]\n\n"
                  << "The commands are:\n" << std::endl;
        auto subs = options.get_subroutine_list();
        for (auto sub : subs) {
          fprintf(stdout, "%s    %s\n", sub.first.c_str(), sub.second.c_str());
        }
        std::cout << std::endl;
      });
}

int main(int argc, const char* argv[]) {
  if (argc > 1) {
    // handle command line execution.
    command_line_handler(argc, argv);
    return 0;
  }
  

  bool isExit = false;
  while (!isExit) {
    std::cout << "////////////////////////////////////" << std::endl;
    const char* options[] = {"EasyPR Option:", "1. ����;",
                             "2. ��������;",   "3. SVMѵ��;",
                             "4. ANNѵ��;",    "5. GDTS����;",
                             "6. �����Ŷ�;",   "7. ��л����;",
                             "8. �˳�;",       NULL};
    easypr::Utils::print_str_lines(options);
    std::cout << "////////////////////////////////////" << std::endl;
    std::cout << "��ѡ��һ�����:";

    int select = -1;
    bool isRepeat = true;
    while (isRepeat) {
      std::cin >> select;
      isRepeat = false;
      switch (select) {
        case 1:
          easypr::demo::testMain();
          break;
        case 2:
          easypr::demo::accuracyTestMain();
          break;
        case 3:
          std::cout << "Run \"demo svm\" for more usage." << std::endl;
          {
            easypr::SvmTrain svm("tmp/svm", "tmp/svm.xml");
            svm.train();
            //easypr::svmTrain(true, false);
          }          
          break;
        case 4:
          std::cout << "Run \"demo ann\" for more usage." << std::endl;
          {
            easypr::AnnTrain ann("tmp/ann", "tmp/ann.xml");
            ann.train();
          }
          break;
        case 5:
          easypr::preprocess::generate_gdts();
          break;
        case 6: {
          // �����Ŷ�;
          // ��ʱ������ӦƸ��Ϣ��лл��
          std::cout << std::endl;
          const char* recruitment[] = {              
              "����EasyPR�Ŷ�Ŀǰ��һ��5�����ҵ�С���ڽ���EasyPR�����汾�Ŀ���"
              "������",
              "������������ʱ������ӦƸ��Ϣ��лл��",
              //"�����Ա���Ŀ����Ȥ������Ը��Ϊ��Դ����һ�����������Ǻܻ�ӭ���"
              //"���롣",
              //"Ŀǰ��Ƹ����Ҫ�˲��ǣ����ƶ�λ��ͼ��ʶ�����ѧϰ����վ�������"
              //"�����ţ�ˡ�",
              //"���������Լ������������뷢�ʼ�����ַ(easypr_dev@163.com)"
              //"���ڴ���ļ��룡",
              NULL};
          easypr::Utils::print_str_lines(recruitment);
          std::cout << std::endl;
          break;
        }
        case 7: {
          // ��л����
          std::cout << std::endl;
          const char* thanks[] = {
              "����Ŀ�ڽ�������У��ܵ��˺ܶ��˵İ��������������ǶԱ���Ŀ����ͻ"
              "�����׵�",
              "(���װ������潨�飬������ţ������ṩ�ȵ�,������ʱ��˳��)��",
              "taotao1233�����ɽ���ƴ�����jsxyhelu�������һ��(zhoushiwei)��",
              "ѧϰ�ܶ���Ԭ��־��ʥ��Сʯ����goldriver��Micooz������ʱ�⣬",
              "Rain Wang��ahccoms����ҹ�䳾������¸�",
              "���кܶ��ͬѧ�Ա���ĿҲ�����˹�����֧�֣��ڴ�Ҳһ����ʾ��ϵ�л"
              "�⣡",
              NULL};
          easypr::Utils::print_str_lines(thanks);
          std::cout << std::endl;
          break;
        }
        case 8:
          isExit = true;
          break;
        default:
          std::cout << "�����������������:";
          isRepeat = true;
          break;
      }
    }
  }
  return 0;
}