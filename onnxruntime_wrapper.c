#include "onnxruntime_wrapper.h"

static const OrtApi *ort_api = NULL;

int SetAPIFromBase(OrtApiBase *api_base) {
  if (!api_base) return 1;
  ort_api = api_base->GetApi(ORT_API_VERSION);
  if (!ort_api) return 2;
  return 0;
}

void ReleaseOrtStatus(OrtStatus *status) {
  ort_api->ReleaseStatus(status);
}

OrtStatus *CreateOrtEnv(char *name, OrtEnv **env) {
  return ort_api->CreateEnv(ORT_LOGGING_LEVEL_ERROR, name, env);
}

void ReleaseOrtEnv(OrtEnv *env) {
  ort_api->ReleaseEnv(env);
}

OrtStatus *CreateOrtMemoryInfo(OrtMemoryInfo **mem_info) {
  return ort_api->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault,
    mem_info);
}

void ReleaseOrtMemoryInfo(OrtMemoryInfo *info) {
  ort_api->ReleaseMemoryInfo(info);
}

const char *GetErrorMessage(OrtStatus *status) {
  if (!status) return "No error (NULL status)";
  return ort_api->GetErrorMessage(status);
}

OrtStatus *CreateSimpleSession(void *model_data, size_t model_data_length,
  OrtEnv *env, OrtSession **out) {
  OrtStatus *status = NULL;
  OrtSessionOptions *options = NULL;
  status = ort_api->CreateSessionOptions(&options);
  if (status) return status;
  status = ort_api->CreateSessionFromArray(env, model_data, model_data_length,
    options, out);
  // It's OK to release the session options now, right? The docs don't say.
  ort_api->ReleaseSessionOptions(options);
  return status;
}

OrtStatus *RunSimpleSession(OrtSession *session, OrtValue *input,
  OrtValue *output) {
  // TODO (next): We actually *do* need to pass in these dang input names.
  const char *input_name[] = {"1x4 Input Vector"};
  const char *output_name[] = {"1x2 Output Vector"};
  OrtStatus *status = NULL;
  status = ort_api->Run(session, NULL, input_name,
    (const OrtValue* const*) &input, 1, output_name, 1, &output);
  return status;
}

void ReleaseOrtSession(OrtSession *session) {
  ort_api->ReleaseSession(session);
}

void ReleaseOrtValue(OrtValue *value) {
  ort_api->ReleaseValue(value);
}

OrtStatus *CreateOrtTensorWithShape(void *data, size_t data_size,
  int64_t *shape, int64_t shape_size, OrtMemoryInfo *mem_info,
  ONNXTensorElementDataType dtype, OrtValue **out) {
  OrtStatus *status = NULL;
  status = ort_api->CreateTensorWithDataAsOrtValue(mem_info, data, data_size,
    shape, shape_size, dtype, out);
  return status;
}
