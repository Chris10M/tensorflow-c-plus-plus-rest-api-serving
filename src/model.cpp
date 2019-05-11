#include <fstream>
#include <iostream>
#include <vector>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "image_utils.hpp"
#include "tf_utils.hpp"
#include "logger.hpp"
#include "utils.hpp"

class TensorFlowModel {
    private:
    TF_Graph* graph;
    
    std::vector<TF_Output> input_ops;
    std::vector<TF_Output> out_ops;
    TF_Session* session; 

    cv::Size model_size;
    std::vector<std::int64_t> input_dims;
    
    int number_of_classes;
    json class_names;

    public:
    TensorFlowModel(std::string model_path, std::vector<std::string> input_node_names, std::vector<std::string> output_node_names) {
        graph = tf_utils::LoadGraph(model_path.c_str());

        for (std::string input_node_name: input_node_names) {
            TF_Output op = {TF_GraphOperationByName(graph, input_node_name.c_str()), 0};
            if (op.oper == nullptr) {
                throw (std::runtime_error("Node " + input_node_name + " not found"));
                exit(0);
            }
            input_ops.push_back(op);
        }
        for (std::string output_node_name: output_node_names) {
            TF_Output op = {TF_GraphOperationByName(graph, output_node_name.c_str()), 0};
            if (op.oper == nullptr) {
                throw (std::runtime_error("Node " + output_node_name + " not found"));
                exit(0);
            }
            out_ops.push_back(op);
        }

        session = tf_utils::CreateSession(graph);
        
        if (session == nullptr) {
            throw (std::runtime_error("Failed to create session"));
            exit(0);  
        }

        model_size = cv::Size(224, 224);
        input_dims = {1, 224, 224, 3};
        number_of_classes = 1000;

        std::ifstream file_stream("./model/imagenet_1000_idx.json");

        if (file_stream.bad()) {
            throw (std::runtime_error("No class index file"));
            exit(0);
        }

        file_stream >> class_names;
    }

    bool is_valid_session() {
        if (session == nullptr)
            return false;
            
        return true;
    }

    std::string process_result(int argmax_value) {
        return class_names[std::to_string(argmax_value)].get<std::string>();
    }
    
    std::vector<float> preprocess_image(cv::Mat image) {
        cv::Mat resized_image;
        cv::Mat rgb_image;
        cv::resize(image, resized_image, model_size);
        cv::cvtColor(resized_image, rgb_image, cv::COLOR_BGR2RGB);

        cv::Mat float_image = ImageUtils::preprocess_image(rgb_image);
        std::vector<float> input_vector = ImageUtils::cv_mat_to_float_array(float_image);

        return input_vector;
    }

    int infer(Logger &logger, cv::Mat image) {
        TimeIt inference_time;

        std::vector<float> input_vector = preprocess_image(image);

        std::vector<TF_Tensor*> input_tensors = {tf_utils::CreateTensor(TF_FLOAT, input_dims, input_vector)};
        std::vector<TF_Tensor*> output_tensors = {nullptr};

        const TF_Code code = tf_utils::RunSession(session, input_ops, input_tensors, out_ops, output_tensors);

        if (code != TF_OK) {
            std::string message = "Run Session Failed";

            logger.error(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="TensorFlowModel.infer";
                                        log.line_number= std::to_string(__LINE__);
                                        //log.request_id = request_id;
                                        log.message=message;
                                    
                                        return log; 
                                  }(Log())
                            );
            
            tf_utils::DeleteTensors(input_tensors);
            tf_utils::DeleteTensors(output_tensors);

            return -1;
        }

        const std::vector<std::vector<float>> data = tf_utils::TensorsData<float>(output_tensors);
        const std::vector<float> result = data[0];

        int argmax_value = ImageUtils::argmax(result);

        logger.info(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="TensorFlowModel.infer";
                                        log.line_number= std::to_string(__LINE__);
                                        //log.request_id = request_id;
                                        log.message="Model Inference Time(ms): " +  std::to_string(inference_time.getDurationInMilliSeconds());
                                    
                                        return log; 
                                  }(Log())
                            );

        tf_utils::DeleteTensors(input_tensors);
        tf_utils::DeleteTensors(output_tensors);

        return argmax_value;
    }

    ~TensorFlowModel() {
        tf_utils::DeleteGraph(graph);
    }
};