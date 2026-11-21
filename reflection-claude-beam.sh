#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODEL=Beam-1.0-501B-A23B
reflection_key=${REFLECTION_API_KEY:-}
if [[ -z $reflection_key && -r "$HOME/.mirror/reflection_api_key" ]]; then
   reflection_key=$(< "$HOME/.mirror/reflection_api_key")
fi
: "${reflection_key:?Set REFLECTION_API_KEY or sign in to Reflection with Mirror}"
unset REFLECTION_API_KEY

proxy_dir=$(mktemp -d)
proxy_pid=
cleanup() {
   if [[ -n $proxy_pid ]]; then
      kill "$proxy_pid" 2>/dev/null || true
      wait "$proxy_pid" 2>/dev/null || true
   fi
   rm -rf -- "$proxy_dir"
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

cat > "$proxy_dir/config.yaml" <<'YAML'
model_list:
  - model_name: Beam-1.0-501B-A23B
    litellm_params:
      model: custom_openai/Beam-1.0-501B-A23B
      api_base: https://api.reflection.ai/openai/v1
      api_key: os.environ/REFLECTION_API_KEY
litellm_settings:
  drop_params: true
  telemetry: false
general_settings:
  master_key: os.environ/LITELLM_MASTER_KEY
YAML

port=${REFLECTION_PROXY_PORT:-4000}
export ANTHROPIC_BASE_URL="http://127.0.0.1:$port"
export ANTHROPIC_AUTH_TOKEN="sk-$(openssl rand -hex 32)"
export ANTHROPIC_API_KEY=
export NO_PROXY="127.0.0.1${NO_PROXY:+,$NO_PROXY}"
export no_proxy="127.0.0.1${no_proxy:+,$no_proxy}"

REFLECTION_API_KEY="$reflection_key" LITELLM_MASTER_KEY="$ANTHROPIC_AUTH_TOKEN" \
   uvx --python 3.12 --from 'litellm[proxy]==1.108.0' litellm \
      --host 127.0.0.1 --port "$port" --config "$proxy_dir/config.yaml" \
      > "$proxy_dir/proxy.log" 2>&1 &
proxy_pid=$!
unset reflection_key

for ((attempt = 0; attempt < 180; ++attempt)); do
   if ! kill -0 "$proxy_pid" 2>/dev/null; then
      cat "$proxy_dir/proxy.log" >&2
      exit 1
   fi
   if curl -fs --max-time 1 -o /dev/null -H "Authorization: Bearer $ANTHROPIC_AUTH_TOKEN" "$ANTHROPIC_BASE_URL/v1/models"; then
      export CLAUDE_CODE_MAX_CONTEXT_TOKENS=1048576
      export ANTHROPIC_DEFAULT_OPUS_MODEL=$MODEL ANTHROPIC_DEFAULT_SONNET_MODEL=$MODEL ANTHROPIC_DEFAULT_HAIKU_MODEL=$MODEL
      export API_TIMEOUT_MS=1800000
      cd "$SCRIPT_DIR"
      ./caffeinate.sh claude --model "$MODEL" "$@"
      exit
   fi
   sleep 1
done

cat "$proxy_dir/proxy.log" >&2
printf '%s\n' 'Timed out waiting for the local Reflection proxy' >&2
exit 1
